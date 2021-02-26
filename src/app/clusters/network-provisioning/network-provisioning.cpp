#include "network-provisioning.h"

#include <cstring>

#include <app/im-encoder.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/ConnectivityManager.h>
#include <platform/ThreadStackManager.h>

namespace chip {
namespace app {
namespace cluster {
namespace NetworkProvisioning {

namespace {
DeviceLayer::Internal::DeviceNetworkProvisioningDelegate * sDelegate = nullptr;
}

void SetDeviceNetworkProvisioningDelegate(DeviceLayer::Internal::DeviceNetworkProvisioningDelegate * delegate)
{
    sDelegate = delegate;
}

// TODO: Use attribute read / write & store

constexpr uint8_t kMaxNetworkIDLen       = 32;
constexpr uint8_t kMaxThreadDatasetLen   = 128;
constexpr uint8_t kMaxWiFiSSIDLen        = 32;
constexpr uint8_t kMaxWiFiCredentialsLen = 64;
constexpr uint8_t kMaxNetworks           = 4;
constexpr char kThreadNetworkName[]      = "tmp-thread-network";

enum class NetworkProvisioningError : uint8_t
{
    kSuccess                = 0,
    kOutOfRange             = 1,
    kBoundsExceeded         = 2,
    kNetworkIDNotFound      = 3,
    kDuplicateNetworkID     = 4,
    kNetworkNotFound        = 5,
    kRegulatoryError        = 6,
    kAuthFailure            = 7,
    kUnsupportedSecurity    = 8,
    kOtherConnectionFailure = 9,
    kIPV6Failed             = 10,
    kIPBindFailed           = 11,
    kLabel9                 = 12,
    kLabel10                = 13,
    kLabel11                = 14,
    kLabel12                = 15,
    kLabel13                = 16,
    kLabel14                = 17,
    kLabel15                = 18,
    kUnknownError           = 19,
};

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

    // VerifyOrExit(DeviceLayer::ConnectivityMgr().IsThreadApplicationControlled(), err = CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);

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

            sNetworks[i].mNetworkType = NetworkType::kThread;
            sNetworks[i].mEnabled     = false;

            memcpy(sNetworks[i].mNetworkID, kThreadNetworkName, sizeof(kThreadNetworkName));
            sNetworks[i].mNetworkIDLen = sizeof(kThreadNetworkName);

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
            memcpy(sNetworks[i].mData.mWiFi.mSSID, ssid, ssidLen);
            sNetworks[i].mData.mWiFi.mSSIDLen = ssidLen;
            memcpy(sNetworks[i].mData.mWiFi.mCredentials, credentials, credentialsLen);
            sNetworks[i].mData.mWiFi.mCredentialsLen = credentialsLen;
            memcpy(sNetworks[i].mNetworkID, sNetworks[i].mData.mWiFi.mSSID, ssidLen);
            sNetworks[i].mNetworkIDLen = ssidLen;

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

void HandleEnableNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    uint8_t networkId[kMaxNetworkIDLen + 1];
    uint32_t networkIdLen;

    TLV::TLVType containerType;
    aReader.EnterContainer(containerType);
    VerifyOrExit(containerType == TLV::TLVType::kTLVType_Structure, err = CHIP_ERROR_INVALID_TLV_TAG);
    while ((err = aReader.Next()) == CHIP_NO_ERROR)
    {
        switch (TLV::TagNumFromTag(aReader.GetTag()))
        {
        case 0:
            VerifyOrExit(aReader.GetType() == TLV::TLVType::kTLVType_ByteString, err = CHIP_ERROR_WRONG_TLV_TYPE);
            networkIdLen = aReader.GetLength();
            VerifyOrExit(networkIdLen < sizeof(networkId), err = CHIP_ERROR_MESSAGE_TOO_LONG);
            SuccessOrExit(err = aReader.GetBytes(networkId, networkIdLen));
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

    size_t networkSeq;

    for (networkSeq = 0; networkSeq < kMaxNetworks; networkSeq++)
    {
        if (sNetworks[networkSeq].mNetworkIDLen == networkIdLen && sNetworks[networkSeq].mNetworkType != NetworkType::kUndefined &&
            memcmp(sNetworks[networkSeq].mNetworkID, networkId, networkIdLen) == 0)
        {
            break;
        }
    }

    VerifyOrExit(networkSeq != kMaxNetworks, err = CHIP_ERROR_KEY_NOT_FOUND);

    ChipLogDetail(Zcl, "Get network: %" PRIuMAX, networkSeq);

    // Just ignore if the network is already enabled
    VerifyOrExit(!sNetworks[networkSeq].mEnabled, err = CHIP_NO_ERROR);
    VerifyOrExit(sDelegate != nullptr, err = CHIP_ERROR_INCORRECT_STATE);

    ChipLogDetail(Zcl, "Enable network");

    switch (sNetworks[networkSeq].mNetworkType)
    {
    case NetworkType::kThread:
        SuccessOrExit(err = sDelegate->ProvisionThread(sNetworks[networkSeq].mData.mThread.mDataset,
                                                       sNetworks[networkSeq].mData.mThread.mDatasetLen));
        break;
    case NetworkType::kWiFi:
        SuccessOrExit(err =
                          sDelegate->ProvisionWiFi(reinterpret_cast<const char *>(sNetworks[networkSeq].mData.mWiFi.mSSID),
                                                   reinterpret_cast<const char *>(sNetworks[networkSeq].mData.mWiFi.mCredentials)));
        break;
    default:
        ExitNow(err = CHIP_ERROR_NOT_IMPLEMENTED);
    }

    sNetworks[networkSeq].mEnabled = true;

exit:
    // TODO: This does not match spec, it should be fixed when fabric provisioning is ready.
    switch (err)
    {
    case CHIP_NO_ERROR:
        EncodeEnableNetworkRespCommand(apCommandObj, 1, 0, 0, "CHIP_NO_ERROR");
        break;
    case CHIP_ERROR_KEY_NOT_FOUND:
        EncodeEnableNetworkRespCommand(apCommandObj, 1, 0, uint8_t(NetworkProvisioningError::kNetworkIDNotFound),
                                       "NetworkIDNetFound");
        break;
    default:
        EncodeEnableNetworkRespCommand(apCommandObj, 1, 0, 19, chip::ErrorStr(err));
        break;
    }
    ChipLogFunctError(err);
}

void HandleGetLastNetworkProvisioningResultCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleRemoveNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleScanNetworksCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleTestNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleUpdateThreadNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleUpdateWiFiNetworkCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

void HandleAddThreadNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleAddWiFiNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleDisableNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleEnableNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleRemoveNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleScanNetworksRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleTestNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleUpdateThreadNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}
void HandleUpdateWiFiNetworkRespCommandReceived(chip::TLV::TLVReader & aReader, chip::app::Command * apCommandObj) {}

} // namespace NetworkProvisioning
} // namespace cluster
} // namespace app
} // namespace chip
