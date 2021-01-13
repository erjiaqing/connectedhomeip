#include <app/im-encoder.h>

#include <cstring>

#include <platform/ConnectivityManager.h>

namespace chip {
namespace app {
namespace cluster {
namespace NetworkProvisioning {

// TODO: Use attribute read / write & store

constexpr uint8_t kMaxNetworkIDLen       = 32;
constexpr uint8_t kMaxThreadDatasetLen   = 128;
constexpr uint8_t kMaxWiFiSSIDLen        = 32;
constexpr uint8_t kMaxWiFiCredentialsLen = 64;
constexpr uint8_t kMaxNetworks           = 4;

enum class NetworkType : uint8_t
{
    kUndefined = 0,
    kWiFi      = 1,
    kThread    = 2,
    kEthernet  = 3,
};

enum class WiFiSecurityType : uint8_t
{
    kUnencrypted  = 0,
    kWEPPersonal  = 1,
    kWPAPersonal  = 2,
    kWPA2Personal = 3,
    kWPA3Personal = 4,
};

struct ThreadNetworkInfo
{
    uint8_t mDataset[kMaxThreadDatasetLen];
    uint8_t mDatasetLen;
};

struct WiFiNetworkInfo
{
    uint8_t mSSID[kMaxWiFiSSIDLen + 1];
    uint8_t mSSIDLen;
    uint8_t mCredentials[kMaxWiFiCredentialsLen];
    uint8_t mCredentialsLen;
    WiFiSecurityType mSecurity;
};

struct NetworkInfo
{
    uint8_t mNetworkID[kMaxNetworkIDLen];
    uint8_t mNetworkIDLen;
    uint8_t mEnabled;
    NetworkType mNetworkType;
    union NetworkData
    {
        ThreadNetworkInfo mThread;
        WiFiNetworkInfo mWiFi;
    } mData;
};

namespace {
NetworkInfo sNetworks[kMaxNetworks];
}

void HandleAddThreadNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    uint8_t operationalDataset[128];
    uint32_t operationalDatasetLen;
    TLV::TLVType containerType;

    VerifyOrExit(DeviceLayer::ConnectivityMgr().IsThreadApplicationControlled(), err = CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);

    aReader.EnterContainer(containerType);
    VerifyOrExit(containerType == TLV::TLVType::kTLVType_Structure, err = CHIP_ERROR_INVALID_TLV_TAG);
    while ((err = aReader.Next()) == CHIP_NO_ERROR)
    {
        uint64_t tag = aReader.GetTag();
        TLV::TLVType elementType;
        switch (TLV::TagNumFromTag(tag))
        {
        case 0:
            elementType = aReader.GetType();
            VerifyOrExit(elementType == TLV::TLVType::kTLVType_ByteString, err = CHIP_ERROR_WRONG_TLV_TYPE);
            operationalDatasetLen = aReader.GetLength();
            VerifyOrExit(operationalDatasetLen <= sizeof(operationalDataset), err = CHIP_ERROR_MESSAGE_TOO_LONG);
            SuccessOrExit(aReader.GetBytes(operationalDataset, operationalDatasetLen));
            break;
        case 1:
        case 2:
            break;
        default:
            err = CHIP_ERROR_UNKNOWN_IMPLICIT_TLV_TAG;
            break;
        }
        SuccessOrExit(err);
    }
    if (err == CHIP_END_OF_TLV)
    {
        err = CHIP_NO_ERROR;
    }

    err = CHIP_ERROR_NO_MEMORY;

    for (size_t i = 0; i < kMaxNetworks; i++)
    {
        if (sNetworks[i].mNetworkType == NetworkType::kUndefined)
        {
            memcpy(sNetworks[i].mData.mThread.mDataset, operationalDataset, operationalDatasetLen);
            sNetworks[i].mData.mThread.mDatasetLen = operationalDatasetLen;

            sNetworks[i].mNetworkType  = NetworkType::kThread;
            sNetworks[i].mEnabled      = false;
            sNetworks[i].mNetworkId    = i & 0xff;
            sNetworks[i].mNetworkIdLen = 1;

            err = CHIP_NO_ERROR;
            break;
        }
    }

    SuccessOrExit(err);

    ChipLogDetail(Zcl, "%s", __FUNCTION__);
exit:
    if (err == CHIP_NO_ERROR)
    {
        EncodeAddThreadNetworkRespCommand(apCommandObj, 1, 0, 0, "CHIP_NO_ERROR");
    }
    else
    {
        EncodeAddThreadNetworkRespCommand(apCommandObj, 1, 0, 19, chip::ErrorStr(err));
    }
    ChipLogFunctError(err);
}

void HandleAddWiFiNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    uint8_t ssid[DeviceLayer::Internal::kMaxWiFiSSIDLength + 1];
    uint32_t ssidLen;
    uint8_t credentials[DeviceLayer::Internal::kMaxWiFiKeyLength];
    uint32_t credentialsLen;

    TLV::TLVType containerType;
    aReader.EnterContainer(containerType);
    VerifyOrExit(containerType == TLV::TLVType::kTLVType_Structure, err = CHIP_ERROR_INVALID_TLV_TAG);
    while ((err = aReader.Next()) == CHIP_NO_ERROR)
    {
        switch (TLV::TagNumFromTag(aReader.GetTag()))
        {
        case 0:
            VerifyOrExit(aReader.GetType() == TLV::TLVType::kTLVType_ByteString, err = CHIP_ERROR_WRONG_TLV_TYPE);
            ssidLen = aReader.GetLength();
            VerifyOrExit(ssidLen < sizeof(ssid), err = CHIP_ERROR_MESSAGE_TOO_LONG);
            SuccessOrExit(err = aReader.GetBytes(ssid, ssidLen));
            break;
        case 1:
            VerifyOrExit(aReader.GetType() == TLV::TLVType::kTLVType_ByteString, err = CHIP_ERROR_WRONG_TLV_TYPE);
            credentialsLen = aReader.GetLength();
            VerifyOrExit(credentialsLen <= sizeof(credentials), err = CHIP_ERROR_MESSAGE_TOO_LONG);
            SuccessOrExit(err = aReader.GetBytes(credentials, credentialsLen));
            break;
        // ProvisionWiFiNetwork
        case 2:
        case 3:
            break;
        default:
            err = CHIP_ERROR_UNKNOWN_IMPLICIT_TLV_TAG;
            break;
        }
        SuccessOrExit(err);
    }
    if (err == CHIP_END_OF_TLV)
    {
        err = CHIP_NO_ERROR;
    }

    err = CHIP_ERROR_NO_MEMORY;

    for (size_t i = 0; i < kMaxNetworks; i++)
    {
        if (sNetworks[i].mNetworkType == NetworkType::kUndefined)
        {
            strncpy(sNetworks[i].mData.mWiFi.mSSID, ssid, ssidLen);
            sNetworks[i].mData.mWiFi.mSSIDLen = ssidLen;
            memcpy(sNetworks[i].mData.mWiFi.mCredentials, credentials, credentialsLen);
            sNetworks[i].mData.mWiFi.mCredentialsLen = credentialsLen;
            strncpy(sNetworks[i].mNetworkId, sNetworks[i].mData.mWiFi.mSSID, ssidLen);
            sNetworks[i].mNetworkIdLen = ssidLen;

            sNetworks[i].mNetworkType = NetworkType::kWiFi;
            sNetworks[i].mEnabled     = false;

            err = CHIP_NO_ERROR;
            break;
        }
    }

    SuccessOrExit(err);

    ChipLogDetail(Zcl, "WiFi provisioning data: SSID: %s", ssid);

    ChipLogDetail(Zcl, "%s", __FUNCTION__);
exit:
    if (err == CHIP_NO_ERROR)
    {
        EncodeAddWiFiNetworkRespCommand(apCommandObj, 1, 0, 0, "CHIP_NO_ERROR");
    }
    else
    {
        EncodeAddWiFiNetworkRespCommand(apCommandObj, 1, 0, 19, chip::ErrorStr(err));
    }
    ChipLogFunctError(err);
}

void HandleDisableNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleEnableNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleGetLastNetworkProvisioningResultCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleRemoveNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleScanNetworksCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleTestNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleUpdateThreadNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleUpdateWiFiNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

} // namespace NetworkProvisioning
} // namespace cluster
} // namespace app
} // namespace chip
