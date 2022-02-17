#ifndef MATTERD_RPC_MATTER_CONTROLLER_H
#define MATTERD_RPC_MATTER_CONTROLLER_H

#include "server.rpc.pb.h"

#include <controller/CHIPDeviceControllerFactory.h>
#include <lib/core/CHIPError.h>

namespace chip {
namespace rpc {

class RPCDevicePairingDelegate final : public Controller::DevicePairingDelegate
{
public:
    ~RPCDevicePairingDelegate() = default;
    void OnPairingComplete(CHIP_ERROR error) override;
    void OnCommissioningComplete(NodeId nodeId, CHIP_ERROR err) override;

    static RPCDevicePairingDelegate sInstance;
};

// Implementation class for MatterController.
class MatterController : public pw_rpc::nanopb::MatterController::Service<MatterController>
{
public:
    ::pw::Status IPCommission(const ::chip_rpc_IPCommissionRequest & request, ::chip_rpc_Status & response);
    ::pw::Status InvokeCommand(const ::chip_rpc_InvokeCommandRequest & request, ::chip_rpc_InvokeCommandResponse & response);

private:
    CHIP_ERROR DoIPCommissioning(const ::chip_rpc_IPCommissionRequest & request);
};

} // namespace rpc
} // namespace chip

#endif // MATTERD_RPC_MATTER_CONTROLLER_H
