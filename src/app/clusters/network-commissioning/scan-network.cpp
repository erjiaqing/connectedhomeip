#include <app/ConcreteCommandPath.h>
#include <app/clusters/network-commissioning/network-commissioning.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::NetworkCommissioning;

namespace {
app::CommandHandler::Handle asyncCommandHandle;

class ScanNetworkCallback : public DeviceLayer::Internal::DeviceNetworkCommissioningDelegate::ScanNetworkCallback
{
public:
    void SetCommandPath(const ConcreteCommandPath & path) { mPath = path; }
    void OnWiFiNetworkDiscovered(const Structs::WiFiInterfaceScanResult::Type & network) override
    {
        for (size_t i = 0; i < CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_WIFI_SCAN_RESULTS; i++)
        {
            if (mWiFiScanResults[i].frequencyBand == 0)
            {
                mWiFiScanResults[i].security      = network.security;
                mWiFiScanResults[i].channel       = network.channel;
                mWiFiScanResults[i].frequencyBand = network.frequencyBand;

                memcpy(mWiFiScanResults[i].ssid, network.ssid.data(), network.ssid.size());
                mWiFiScanResults[i].ssidLen = static_cast<uint8_t>(network.ssid.size());
                memcpy(mWiFiScanResults[i].bssid, network.bssid.data(), network.bssid.size());
                mWiFiScanResults[i].bssidLen = static_cast<uint8_t>(network.bssid.size());
                break;
            }
        }
    }

    void OnThreadNetworkDiscovered(const Structs::ThreadInterfaceScanResult::Type & network) override
    {
        for (size_t i = 0; i < CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_THREAD_SCAN_RESULTS; i++)
        {
            if (mThreadScanResults[i].mDatasetLen == 0)
            {
                memcpy(mThreadScanResults[i].mDataset, network.discoveryResponse.data(), network.discoveryResponse.size());
                mThreadScanResults[i].mDatasetLen = static_cast<uint8_t>(network.discoveryResponse.size());
                break;
            }
        }
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
        Commands::ScanNetworksResponse::Type response;
        response.errorCode = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS;

        Structs::WiFiInterfaceScanResult::Type wifiScanResults[CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_WIFI_SCAN_RESULTS];
        Structs::ThreadInterfaceScanResult::Type threadScanResults[CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_THREAD_SCAN_RESULTS];
        size_t numWiFiNetworkFound = 0, numThreadNetworkFound = 0;
        for (int i = 0; i < CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_WIFI_SCAN_RESULTS; i++)
        {
            if (mWiFiScanResults[i].frequencyBand != 0)
            {
                auto writeItem           = &wifiScanResults[numWiFiNetworkFound];
                writeItem->frequencyBand = mWiFiScanResults[i].frequencyBand;
                writeItem->security      = mWiFiScanResults[i].security;
                writeItem->channel       = mWiFiScanResults[i].channel;
                writeItem->ssid          = ByteSpan(mWiFiScanResults[i].ssid, mWiFiScanResults[i].ssidLen);
                writeItem->bssid         = ByteSpan(mWiFiScanResults[i].bssid, mWiFiScanResults[i].bssidLen);
                numWiFiNetworkFound++;
            }
        }
        for (int i = 0; i < CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_THREAD_SCAN_RESULTS; i++)
        {
            if (mThreadScanResults[i].mDatasetLen != 0)
            {
                threadScanResults[numThreadNetworkFound].discoveryResponse =
                    ByteSpan(mThreadScanResults[i].mDataset, mThreadScanResults[i].mDatasetLen);
            }
        }

        response.wifiScanResults = DataModel::List<Structs::WiFiInterfaceScanResult::Type>(wifiScanResults, numWiFiNetworkFound);
        response.threadScanResults =
            DataModel::List<Structs::ThreadInterfaceScanResult::Type>(threadScanResults, numThreadNetworkFound);
        commandHandle->AddResponseData(mPath, response);
        asyncCommandHandle = nullptr;
    }

private:
    ConcreteCommandPath mPath = ConcreteCommandPath(0, 0, 0);
    WiFiScanInfo mWiFiScanResults[CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_WIFI_SCAN_RESULTS];
    ThreadNetworkInfo mThreadScanResults[CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_THREAD_SCAN_RESULTS];
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
    scanNetworkCallback.SetCommandPath(commandPath);

    commissioningDelegate->ScanNetworks(System::Clock::Milliseconds32(commandData.timeoutMs), &scanNetworkCallback);
    return true;
}
