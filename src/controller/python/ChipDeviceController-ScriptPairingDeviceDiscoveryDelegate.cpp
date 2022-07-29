#include "ChipDeviceController-ScriptPairingDeviceDiscoveryDelegate.h"

namespace chip {
namespace Controller {

void ScriptPairingDeviceDiscoveryDelegate::OnDiscoveredDevice(const Dnssd::DiscoveredNodeData & nodeData)
{
    // Ignore nodes with closed comissioning window
    VerifyOrReturn(nodeData.commissionData.commissioningMode != 0);
    VerifyOrReturn(mpActiveDeviceCommissioner != nullptr);

    const uint16_t port = nodeData.resolutionData.port;
    char buf[chip::Inet::IPAddress::kMaxStringLength];
    nodeData.resolutionData.ipAddress[0].ToString(buf);
    ChipLogProgress(chipTool, "Discovered Device: %s:%u", buf, port);

    // Stop Mdns discovery. Is it the right method ?
    mpActiveDeviceCommissioner->RegisterDeviceDiscoveryDelegate(nullptr);

    Inet::InterfaceId interfaceId =
        nodeData.resolutionData.ipAddress[0].IsIPv6LinkLocal() ? nodeData.resolutionData.interfaceId : Inet::InterfaceId::Null();
    PeerAddress peerAddress = PeerAddress::UDP(nodeData.resolutionData.ipAddress[0], port, interfaceId);

    RendezvousParameters keyExchangeParams = RendezvousParameters().SetSetupPINCode(mSetupPINCode).SetPeerAddress(peerAddress);

    CHIP_ERROR err = mpActiveDeviceCommissioner->PairDevice(mNodeId, keyExchangeParams, mParams);
    if (err != CHIP_NO_ERROR)
    {
        VerifyOrReturn(mpPairingDelegate != nullptr);
        mpPairingDelegate->OnCommissioningComplete(mNodeId, err);
    }
}
} // namespace Controller
} // namespace chip
