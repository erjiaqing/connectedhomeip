#include "matter_controller.h"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>

#include <app/OperationalDeviceProxy.h>
#include <controller/CHIPDeviceControllerFactory.h>
#include <lib/support/logging/CHIPLogging.h>
#include <rpc.h>

using namespace std;
using namespace std::chrono_literals;

using namespace chip;
using namespace chip::app;

namespace chip {
namespace rpc {

RPCDevicePairingDelegate RPCDevicePairingDelegate::sInstance;

namespace {
bool isPairing;
Optional<uint32_t> pairingStatus;
mutex pairingStatusMutex;
condition_variable pairingStatusCV;
} // namespace

void RPCDevicePairingDelegate::OnPairingComplete(CHIP_ERROR error) {}

void RPCDevicePairingDelegate::OnCommissioningComplete(NodeId nodeId, CHIP_ERROR err)
{
    unique_lock<mutex> lk(pairingStatusMutex);
    pairingStatus.SetValue(err.AsInteger());
    pairingStatusCV.notify_one();
}

// Method definitions for MatterController.

CHIP_ERROR MatterController::DoIPCommissioning(const ::chip_rpc_IPCommissionRequest & request)
{
    chip::Inet::IPAddress peerAddr;
    chip::Transport::PeerAddress addr;

    chip::RendezvousParameters params = chip::RendezvousParameters().SetSetupPINCode(request.Pincode);

    VerifyOrReturnError(chip::Inet::IPAddress::FromString(request.IP, peerAddr), CHIP_ERROR_INVALID_ARGUMENT);
    // TODO: IP rendezvous should use TCP connection.
    addr.SetTransportType(chip::Transport::Type::kUdp).SetIPAddress(peerAddr);
    params.SetPeerAddress(addr).SetDiscriminator(0);
    return RunOnMatterThreadAndWait<CHIP_ERROR>(
        [&request, &params]() -> CHIP_ERROR { return RPCServer::GetInstance()->controller->PairDevice(request.NodeId, params); });
}

::pw::Status MatterController::IPCommission(const ::chip_rpc_IPCommissionRequest & request, ::chip_rpc_Status & response)
{
    // TODO: Read the request as appropriate for your application
    static_cast<void>(request);
    // TODO: Fill in the response as appropriate for your application
    static_cast<void>(response);
    ChipLogError(Controller, "MatterController::IPCommission");
    unique_lock<mutex> lk(pairingStatusMutex);

    if (isPairing)
    {
        return ::pw::Status::ResourceExhausted();
    }

    CHIP_ERROR err = DoIPCommissioning(request);
    if (err == CHIP_NO_ERROR)
    {
        pairingStatusCV.wait_until(lk, chrono::system_clock::now() + 60s, []() { return pairingStatus.HasValue(); });
        response.Status = pairingStatus.Value();
    }
    else
    {
        response.Status = 1;
    }
    isPairing = false;

    return ::pw::Status();
}

namespace {
class CommandSenderCallback : public CommandSender::Callback
{
public:
    using OnSuccessCb = std::function<void(const ConcreteCommandPath &, const app::StatusIB &, const uint8_t *, const size_t)>;
    using OnErrorCb   = std::function<void(const CHIP_ERROR)>;

    CommandSenderCallback(OnSuccessCb onSuccess, OnErrorCb onError) : onSuccess_(onSuccess), onError_(onError) {}

    void OnResponse(CommandSender * apCommandSender, const ConcreteCommandPath & aPath, const app::StatusIB & aStatus,
                    TLV::TLVReader * aData) override
    {
        uint8_t buffer[CHIP_CONFIG_DEFAULT_UDP_MTU_SIZE];
        uint32_t size = 0;
        // When the apData is nullptr, means we did not receive a valid attribute data from server, status will be some error
        // status.
        if (aData != nullptr)
        {
            // Python need to read from full TLV data the TLVReader may contain some unclean states.
            TLV::TLVWriter writer;
            writer.Init(buffer);
            CHIP_ERROR err = writer.CopyContainer(TLV::AnonymousTag(), *aData);
            if (err != CHIP_NO_ERROR)
            {
                onError_(err);
                return;
            }
            size = writer.GetLengthWritten();
        }

        onSuccess_(aPath, aStatus, buffer, size);
    }

    void OnError(const CommandSender * apCommandSender, CHIP_ERROR aProtocolError) override
    {
        onError_(aProtocolError);
    }

    void OnDone(CommandSender * apCommandSender) override
    {
        delete apCommandSender;
        delete this;
    };

private:
    OnSuccessCb onSuccess_;
    OnErrorCb onError_;
};

CHIP_ERROR DoSendCommand(OperationalDeviceProxy * device, chip::EndpointId endpointId, chip::ClusterId clusterId,
                         chip::CommandId commandId, const uint8_t * payload, size_t length,
                         CommandSenderCallback::OnSuccessCb onSuccess, CommandSenderCallback::OnErrorCb onError)
{
    std::unique_ptr<CommandSenderCallback> callback = std::make_unique<CommandSenderCallback>(onSuccess, onError);
    std::unique_ptr<CommandSender> sender = std::make_unique<CommandSender>(callback.get(), device->GetExchangeManager(), false);

    app::CommandPathParams cmdParams = { endpointId, /* group id */ 0, clusterId, commandId,
                                         (app::CommandPathFlags::kEndpointIdValid) };

    CHIP_ERROR err = CHIP_NO_ERROR;

    SuccessOrExit(err = sender->PrepareCommand(cmdParams, false));

    {
        auto writer = sender->GetCommandDataIBTLVWriter();
        TLV::TLVReader reader;
        VerifyOrExit(writer != nullptr, err = CHIP_ERROR_INCORRECT_STATE);
        reader.Init(payload, length);
        reader.Next();
        SuccessOrExit(writer->CopyContainer(TLV::ContextTag(to_underlying(CommandDataIB::Tag::kData)), reader));
    }

    SuccessOrExit(err = sender->FinishCommand(Optional<uint16_t>::Missing()));

    SuccessOrExit(err = device->SendCommands(sender.get()));

    sender.release();
    callback.release();
exit:
    return err;
}

struct GetDeviceCallbacks
{
    using DeviceAvailableFunc = std::function<void(OperationalDeviceProxy *, CHIP_ERROR)>;

    GetDeviceCallbacks(DeviceAvailableFunc callback) :
        mOnSuccess(OnDeviceConnectedFn, this), mOnFailure(OnConnectionFailureFn, this), mCallback(callback)
    {}

    static void OnDeviceConnectedFn(void * context, OperationalDeviceProxy * device)
    {
        auto * self = static_cast<GetDeviceCallbacks *>(context);
        self->mCallback(device, CHIP_NO_ERROR);
        delete self;
    }

    static void OnConnectionFailureFn(void * context, PeerId peerId, CHIP_ERROR error)
    {
        auto * self = static_cast<GetDeviceCallbacks *>(context);
        self->mCallback(nullptr, error);
        delete self;
    }

    Callback::Callback<OnDeviceConnected> mOnSuccess;
    Callback::Callback<OnDeviceConnectionFailure> mOnFailure;
    DeviceAvailableFunc mCallback;
};

CHIP_ERROR FindOrEstablishConnection(NodeId nodeid, chip::OperationalDeviceProxy ** device)
{
    mutex getDeviceMutex;
    condition_variable getDeviceCV;

    CHIP_ERROR ret;

    bool isFinished = false;

    auto callback = [&device, &ret, &isFinished, &getDeviceMutex, &getDeviceCV](OperationalDeviceProxy * proxy, CHIP_ERROR err) {
        ret     = err;
        *device = proxy;
        {
            unique_lock<mutex> lk(getDeviceMutex);
            isFinished = true;
            getDeviceCV.notify_one();
        }
    };

    auto * callbacks = new GetDeviceCallbacks(callback);

    ret = RunOnMatterThreadAndWait<CHIP_ERROR>([nodeid, callbacks]() -> CHIP_ERROR {
        return RPCServer::GetInstance()->controller->GetConnectedDevice(nodeid, &callbacks->mOnSuccess, &callbacks->mOnFailure);
    });

    if (ret == CHIP_NO_ERROR)
    {
        unique_lock<mutex> lk(getDeviceMutex);
        getDeviceCV.wait_until(lk, chrono::system_clock::now() + 60s, [&isFinished]() { return isFinished; });
    }
    else
    {
        delete callbacks;
    }

    return ret;
}
} // namespace

::pw::Status MatterController::InvokeCommand(const ::chip_rpc_InvokeCommandRequest & request,
                                             ::chip_rpc_InvokeCommandResponse & response)
{
    mutex invokeCommandMutex;
    condition_variable invokeCommandCV;
    bool commandDone = false;
    OperationalDeviceProxy * device;

    CHIP_ERROR err = FindOrEstablishConnection(request.NodeId, &device);

    if (err != CHIP_NO_ERROR)
    {
        return ::pw::Status::NotFound();
    }

    auto onSuccess = [&response, &invokeCommandMutex, &commandDone, &invokeCommandCV](const ConcreteCommandPath & path,
                                                                                      const app::StatusIB & status,
                                                                                      const uint8_t * data, const size_t dataLen) {
        response.has_path        = true;
        response.path.EndpointID = path.mEndpointId;
        response.path.ClusterID  = path.mClusterId;
        response.path.CommandID  = path.mCommandId;
        response.has_statusCode  = true;
        response.statusCode      = to_underlying(status.mStatus);
        response.has_payload     = (data != nullptr);
        if (data != nullptr)
        {
            memcpy(response.payload.bytes, data, dataLen);
            response.payload.size = dataLen;
        }

        unique_lock<mutex> lk(invokeCommandMutex);
        commandDone = true;
        invokeCommandCV.notify_one();
    };
    auto onError = [&err, &invokeCommandMutex, &commandDone, &invokeCommandCV](CHIP_ERROR err2) {
        err = err2;
        unique_lock<mutex> lk(invokeCommandMutex);
        commandDone = true;
        invokeCommandCV.notify_one();
    };

    unique_lock<mutex> lk(invokeCommandMutex);

    err = RunOnMatterThreadAndWait<CHIP_ERROR>([&]() -> CHIP_ERROR {
        return DoSendCommand(device, request.path.EndpointID, request.path.ClusterID, request.path.CommandID, request.payload.bytes,
                             request.payload.size, onSuccess, onError);
    });

    if (err == CHIP_NO_ERROR)
    {
        invokeCommandCV.wait_until(lk, chrono::system_clock::now() + 60s, [&commandDone]() { return commandDone; });
    }

    if (err == CHIP_NO_ERROR)
    {
        return ::pw::Status();
    }
    else
    {
        return ::pw::Status::Internal();
    }
}

} // namespace rpc
} // namespace chip
