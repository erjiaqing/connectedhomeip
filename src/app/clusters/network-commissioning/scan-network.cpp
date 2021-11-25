#include <app/ConcreteCommandPath.h>
#include <app/StatusResponse.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::NetworkCommissioning;

namespace {
// The WiFiInterfaceScanResult will use 60 bytes on wire, so 15 is a safe upperbound for encoding ScanNetworkResponse for WiFi.
// The WiFiInterfaceScanResult layout on wire is:
// 1 byte struct begin
//   3 bytes secutiyy (Type-Tag-Value)
//   max. 35 bytes ssid (Type-Tag-Length-Value-EndOfContainer)
//   9 bytes bssid (Type-Tag-Length-Value-EndOfContainer)
//   3 bytes channel
//   6 bytes frequency band
//   3 bytes rssi
// 1 byte struct end
constexpr size_t kMaxWiFiNetworkInScanResponse = 15;

app::CommandHandler::Handle asyncCommandHandle;

class ScanWiFiNetworkCallback : public DeviceLayer::Internal::DeviceNetworkCommissioningDelegate::ScanWiFiNetworkCallback
{
public:
    void Init(const ConcreteCommandPath & path) { mPath = path; }
    void OnScanFinished(const Span<Structs::WiFiInterfaceScanResult::Type> & network) override
    {
        auto commandHandleRef = std::move(asyncCommandHandle);
        auto commandHandle    = commandHandleRef.Get();
        if (commandHandle == nullptr)
        {
            // Oh, this is a response of a network scan from unclean shutdown, ignore it.
            return;
        }

        Commands::ScanNetworksResponse::Type response;
        response.errorCode       = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS;
        response.wifiScanResults = DataModel::List<Structs::WiFiInterfaceScanResult::Type>(
            network.data(), network.size() < kMaxWiFiNetworkInScanResponse ? network.size() : kMaxWiFiNetworkInScanResponse);
        commandHandle->AddResponseData(mPath, response);
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

private:
    ConcreteCommandPath mPath = ConcreteCommandPath(0, 0, 0);
};

ScanWiFiNetworkCallback scanWiFiNetworkCallback;
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
    scanWiFiNetworkCallback.Init(commandPath);

    commissioningDelegate->ScanWiFiNetworks(System::Clock::Milliseconds32(commandData.timeoutMs), commandData.ssid,
                                            &scanWiFiNetworkCallback);
    return true;
}
