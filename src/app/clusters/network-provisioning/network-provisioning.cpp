/**
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "network-provisioning.h"

#include <cstring>

#include <platform/CHIPDeviceLayer.h>
#include <platform/ConnectivityManager.h>
#include <platform/ThreadStackManager.h>
#include <platform/internal/DeviceNetworkProvisioning.h>

using namespace chip::DeviceLayer;

namespace chip {
namespace app {
namespace cluster {
namespace NetworkProvisioning {

// TODO: DeviceNetworkProvisioningDelegate should be deprecated, network provisioning can call related functions in device layer
// directly.
namespace {
DeviceLayer::Internal::DeviceNetworkProvisioningDelegate * sDelegate = nullptr;
}

void SetDeviceNetworkProvisioningDelegate(chip::DeviceLayer::Internal::DeviceNetworkProvisioningDelegate * delegate)
{
    sDelegate = delegate;
}

constexpr uint8_t kMaxNetworkIDLen       = 32;
constexpr uint8_t kMaxThreadDatasetLen   = 128;
constexpr uint8_t kMaxWiFiSSIDLen        = 32;
constexpr uint8_t kMaxWiFiCredentialsLen = 64;
constexpr uint8_t kMaxNetworks           = 4;

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

// TODO: Use attribute read / write & store

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

void HandleAddThreadNetworkCommandReceived(app::Command * apCommandObj, chip::EndpointId aEndpoint,
                                           const uint8_t * operationalDataset, const uint32_t operationalDatasetLen,
                                           uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    VerifyOrExit(DeviceLayer::ConnectivityMgr().IsThreadApplicationControlled(), err = CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);

    err = CHIP_ERROR_NO_MEMORY;

    for (size_t i = 0; i < kMaxNetworks; i++)
    {
        if (sNetworks[i].mNetworkType == NetworkType::kUndefined)
        {
            memcpy(sNetworks[i].mData.mThread.mDataset, operationalDataset, operationalDatasetLen);
            sNetworks[i].mData.mThread.mDatasetLen = operationalDatasetLen;

            sNetworks[i].mNetworkType  = NetworkType::kThread;
            sNetworks[i].mEnabled      = false;
            sNetworks[i].mNetworkID[0] = i & 0xff; // TODO: Should be extpanid from operational dataset
            sNetworks[i].mNetworkIDLen = 1;

            err = CHIP_NO_ERROR;
            break;
        }
    }

    SuccessOrExit(err);

    ChipLogDetail(Zcl, "%s", __FUNCTION__);
exit:
    // TODO: Prepare return command for add thread network.
    ChipLogFunctError(err);
}

void HandleAddWiFiNetworkCommandReceived(app::Command * apCommandObj, chip::EndpointId aEndpoint, const uint8_t * ssid,
                                         const uint32_t ssidLen, const uint8_t * credentials, const uint32_t credentialsLen,
                                         uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

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
exit:
    // TODO: Prepare response command for add WiFi network data.
    ChipLogFunctError(err);
}

void HandleEnableNetworkCommandReceived(app::Command * apCommandObj, chip::EndpointId aEndpoint, const uint8_t * networkId,
                                        const uint32_t networkIdLen, uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

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
        SuccessOrExit(err = ThreadStackMgr().SetThreadProvision(sNetworks[networkSeq].mData.mThread.mDataset,
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
    // TODO: Prepare response for enable network command.
    ChipLogFunctError(err);
}

} // namespace NetworkProvisioning
} // namespace cluster
} // namespace app
} // namespace chip
