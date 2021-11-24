#include <app/ConcreteCommandPath.h>
#include <app/StatusResponse.h>
#include <app/clusters/network-commissioning/network-commissioning.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::NetworkCommissioning;

namespace {
app::CommandHandler::Handle asyncCommandHandle;

class ScanNetworkCallback : public DeviceLayer::Internal::DeviceNetworkCommissioningDelegate::ScanNetworkCallback
{
private:
    static constexpr TLV::Tag kWifiNetworkTag   = TLV::ContextTag(0);
    static constexpr TLV::Tag kThreadNetworkTag = TLV::ContextTag(1);

public:
    CHIP_ERROR Init(const ConcreteCommandPath & path)
    {
        chip::System::PacketBufferHandle bufHandle = System::PacketBufferHandle::New(chip::app::kMaxSecureSduLengthBytes);
        mPath                                      = path;
        VerifyOrReturnError(!bufHandle.IsNull(), CHIP_ERROR_NO_MEMORY);
        mScanResults.Init(std::move(bufHandle));
        return mScanResults.StartContainer(TLV::AnonymousTag, TLV::kTLVType_List, mDummyType);
    }
    void OnWiFiNetworkDiscovered(const Structs::WiFiInterfaceScanResult::Type & network) override
    {
        DataModel::Encode(mScanResults, kWifiNetworkTag, network);
    }

    void OnThreadNetworkDiscovered(const Structs::ThreadInterfaceScanResult::Type & network) override
    {
        DataModel::Encode(mScanResults, kThreadNetworkTag, network);
    }

    void OnError(CHIP_ERROR err) override
    {
        auto commandHandle = asyncCommandHandle.Get();
        if (commandHandle == nullptr)
        {
            // Oh, this is a response of a network scan from unclean shutdown, ignore it.
            return;
        }
        Commands::ScanNetworksResponse::Type response;
        response.errorCode = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_UNKNOWN_ERROR;
        commandHandle->AddResponseData(mPath, response);

        commandHandle = nullptr;
    }

    void OnDone() override
    {
        auto commandHandle = asyncCommandHandle.Get();

        if (commandHandle == nullptr)
        {
            // Oh, this is a response of a network scan from unclean shutdown, ignore it.
            return;
        }

        auto commandHandleRef = std::move(asyncCommandHandle);
        Commands::ScanNetworksResponse::Type response;
        response.errorCode = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS;

        CHIP_ERROR err = mScanResults.EndContainer(mDummyType);

        if (err != CHIP_NO_ERROR)
        {
            response.errorCode = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_UNKNOWN_ERROR;
            commandHandle->AddResponseData(mPath, response);
            return;
        }

        chip::System::PacketBufferHandle bufHandle;
        err = mScanResults.Finalize(&bufHandle);
        if (err != CHIP_NO_ERROR)
        {
            response.errorCode = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_UNKNOWN_ERROR;
            commandHandle->AddResponseData(mPath, response);
            return;
        }

        chip::System::PacketBufferTLVReader reader;
        reader.Init(std::move(bufHandle));

        reader.EnterContainer(mDummyType);
        Structs::WiFiInterfaceScanResult::Type wifiScanResult[CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_WIFI_SCAN_RESULTS];
        Structs::ThreadInterfaceScanResult::Type threadScanResult[CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_THREAD_SCAN_RESULTS];
        size_t numWiFiNetwork = 0, numThreadNetwork = 0;
        while ((err = reader.Next()) == CHIP_NO_ERROR)
        {
            TLV::TLVReader dataDecodeReader;
            dataDecodeReader.Init(reader);
            if (reader.GetTag() == kWifiNetworkTag)
            {
                DataModel::Decode(dataDecodeReader, wifiScanResult[numWiFiNetwork++]);
            }
            else if (reader.GetTag() == kThreadNetworkTag)
            {
                DataModel::Decode(dataDecodeReader, threadScanResult[numThreadNetwork++]);
            }
        }

        response.wifiScanResults   = DataModel::List<Structs::WiFiInterfaceScanResult::Type>(wifiScanResult, numWiFiNetwork);
        response.threadScanResults = DataModel::List<Structs::ThreadInterfaceScanResult::Type>(threadScanResult, numThreadNetwork);
        commandHandle->AddResponseData(mPath, response);
    }

private:
    ConcreteCommandPath mPath = ConcreteCommandPath(0, 0, 0);
    chip::System::PacketBufferTLVWriter mScanResults;
    TLV::TLVType mDummyType;
};

ScanNetworkCallback scanNetworkCallback;
} // namespace

bool emberAfNetworkCommissioningClusterScanNetworksCallback(app::CommandHandler * commandObj,
                                                            const app::ConcreteCommandPath & commandPath,
                                                            const Commands::ScanNetworks::DecodableType & commandData)
{
    if (asyncCommandHandle.Get() != nullptr)
    {
        // We have a command processing in the backend, reject all incoming commands.
        commandObj->AddStatus(commandPath, Protocols::InteractionModel::Status::Busy);
        return true;
    }

    auto commissioningDelegate = Internal::GetDeviceNetworkCommissioningDelegate();
    if (commissioningDelegate == nullptr)
    {
        // The server did not register a commissioning delegate for handling network commissioning commands, abort.
        commandObj->AddStatus(commandPath, Protocols::InteractionModel::Status::Failure);
        return true;
    }

    // Set the current background work to this command.
    asyncCommandHandle = app::CommandHandler::Handle(commandObj);
    scanNetworkCallback.Init(commandPath);

    commissioningDelegate->ScanNetworks(System::Clock::Milliseconds32(commandData.timeoutMs), &scanNetworkCallback);
    return true;
}
