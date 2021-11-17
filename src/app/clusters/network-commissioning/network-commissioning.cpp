/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
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

#include "network-commissioning.h"

#include <cstring>
#include <type_traits>

#include <app-common/zap-generated/cluster-objects.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/SafeInt.h>
#include <lib/support/Span.h>
#include <lib/support/ThreadOperationalDataset.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/ConnectivityManager.h>
#include <platform/internal/DeviceControlServer.h>

using namespace chip;
using namespace chip::app;

namespace chip {
namespace app {
namespace Clusters {
namespace NetworkCommissioning {

namespace {
// The internal network info containing credentials. Need to find some better place to save these info.
NetworkInfo sNetworks[kMaxNetworks];
DeviceLayer::Internal::DeviceNetworkCommissioningDelegate * commissioningDelegate = nullptr;
} // namespace

namespace Internal {
DeviceLayer::Internal::DeviceNetworkCommissioningDelegate * GetDeviceNetworkCommissioningDelegate()
{
    return commissioningDelegate;
}
} // namespace Internal

void OnAddThreadNetworkCommandCallbackInternal(app::CommandHandler * apCommandHandler, const app::ConcreteCommandPath & commandPath,
                                               ByteSpan operationalDataset, uint64_t breadcrumb, uint32_t timeoutMs)
{
    Commands::AddThreadNetworkResponse::Type response;
    EmberAfNetworkCommissioningError err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_BOUNDS_EXCEEDED;

    for (size_t i = 0; i < kMaxNetworks; i++)
    {
        if (sNetworks[i].mNetworkType == NetworkType::kUndefined)
        {
            Thread::OperationalDataset & dataset = sNetworks[i].mData.mThread;
            CHIP_ERROR error                     = dataset.Init(operationalDataset);

            if (error != CHIP_NO_ERROR)
            {
                ChipLogDetail(Zcl, "Failed to parse Thread operational dataset: %s", ErrorStr(error));
                err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_UNKNOWN_ERROR;
                break;
            }

            uint8_t extendedPanId[Thread::kSizeExtendedPanId];

            static_assert(sizeof(sNetworks[i].mNetworkID) >= sizeof(extendedPanId),
                          "Network ID must be larger than Thread extended PAN ID!");
            SuccessOrExit(dataset.GetExtendedPanId(extendedPanId));
            memcpy(sNetworks[i].mNetworkID, extendedPanId, sizeof(extendedPanId));
            sNetworks[i].mNetworkIDLen = sizeof(extendedPanId);

            sNetworks[i].mNetworkType = NetworkType::kThread;
            sNetworks[i].mEnabled     = false;

            err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS;
            break;
        }
    }

exit:
    // TODO: We should encode response command here.

    ChipLogDetail(Zcl, "AddThreadNetwork: %" PRIu8, err);
    response.errorCode = err;
    apCommandHandler->AddResponseData(commandPath, response);
}

void OnAddWiFiNetworkCommandCallbackInternal(app::CommandHandler * apCommandHandler, const app::ConcreteCommandPath & commandPath,
                                             ByteSpan ssid, ByteSpan credentials, uint64_t breadcrumb, uint32_t timeoutMs)
{
    Commands::AddWiFiNetworkResponse::Type response;
    EmberAfNetworkCommissioningError err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_BOUNDS_EXCEEDED;

    for (size_t i = 0; i < kMaxNetworks; i++)
    {
        if (sNetworks[i].mNetworkType == NetworkType::kUndefined)
        {
            VerifyOrExit(ssid.size() <= sizeof(sNetworks[i].mData.mWiFi.mSSID),
                         err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_OUT_OF_RANGE);
            memcpy(sNetworks[i].mData.mWiFi.mSSID, ssid.data(), ssid.size());

            using WiFiSSIDLenType = decltype(sNetworks[i].mData.mWiFi.mSSIDLen);
            VerifyOrExit(CanCastTo<WiFiSSIDLenType>(ssid.size()), err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_OUT_OF_RANGE);
            sNetworks[i].mData.mWiFi.mSSIDLen = static_cast<WiFiSSIDLenType>(ssid.size());

            VerifyOrExit(credentials.size() <= sizeof(sNetworks[i].mData.mWiFi.mCredentials),
                         err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_OUT_OF_RANGE);
            memcpy(sNetworks[i].mData.mWiFi.mCredentials, credentials.data(), credentials.size());

            using WiFiCredentialsLenType = decltype(sNetworks[i].mData.mWiFi.mCredentialsLen);
            VerifyOrExit(CanCastTo<WiFiCredentialsLenType>(ssid.size()), err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_OUT_OF_RANGE);
            sNetworks[i].mData.mWiFi.mCredentialsLen = static_cast<WiFiCredentialsLenType>(credentials.size());

            VerifyOrExit(ssid.size() <= sizeof(sNetworks[i].mNetworkID), err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_OUT_OF_RANGE);
            memcpy(sNetworks[i].mNetworkID, sNetworks[i].mData.mWiFi.mSSID, ssid.size());

            using NetworkIDLenType = decltype(sNetworks[i].mNetworkIDLen);
            VerifyOrExit(CanCastTo<NetworkIDLenType>(ssid.size()), err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_OUT_OF_RANGE);
            sNetworks[i].mNetworkIDLen = static_cast<NetworkIDLenType>(ssid.size());

            sNetworks[i].mNetworkType = NetworkType::kWiFi;
            sNetworks[i].mEnabled     = false;

            err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS;
            break;
        }
    }

    VerifyOrExit(err == EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS, );

    ChipLogDetail(Zcl, "WiFi provisioning data: SSID: %.*s", static_cast<int>(ssid.size()), ssid.data());
exit:
    // TODO: We should encode response command here.

    ChipLogDetail(Zcl, "AddWiFiNetwork: %" PRIu8, err);
    response.errorCode = err;
    apCommandHandler->AddResponseData(commandPath, response);
}

namespace {
CHIP_ERROR DoEnableNetwork(NetworkInfo * network)
{
    switch (network->mNetworkType)
    {
    case NetworkType::kThread:
        ReturnErrorOnFailure(commissioningDelegate->ConnectToThreadNetwork(network->mData.mThread.AsByteSpan()));
        break;
    case NetworkType::kWiFi:
        ReturnErrorOnFailure(
            commissioningDelegate->ProvisionWiFi(reinterpret_cast<const char *>(network->mData.mWiFi.mSSID),
                                                 reinterpret_cast<const char *>(network->mData.mWiFi.mCredentials)));
        break;
    case NetworkType::kEthernet:
    case NetworkType::kUndefined:
    default:
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }
    network->mEnabled = true;
    return CHIP_NO_ERROR;
}
} // namespace

void OnEnableNetworkCommandCallbackInternal(app::CommandHandler * apCommandHandler, const app::ConcreteCommandPath & commandPath,
                                            ByteSpan networkID, uint64_t breadcrumb, uint32_t timeoutMs)
{
    Commands::EnableNetworkResponse::Type response;
    size_t networkSeq;
    EmberAfNetworkCommissioningError err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_NETWORK_ID_NOT_FOUND;

    for (networkSeq = 0; networkSeq < kMaxNetworks; networkSeq++)
    {
        if (sNetworks[networkSeq].mNetworkIDLen == networkID.size() &&
            sNetworks[networkSeq].mNetworkType != NetworkType::kUndefined &&
            memcmp(sNetworks[networkSeq].mNetworkID, networkID.data(), networkID.size()) == 0)
        {
            // TODO: Currently, we cannot figure out the detailed error from network provisioning on DeviceLayer, we should
            // implement this in device layer.
            VerifyOrExit(DoEnableNetwork(&sNetworks[networkSeq]) == CHIP_NO_ERROR,
                         err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_UNKNOWN_ERROR);
            ExitNow(err = EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS);
        }
    }
    // TODO: We should encode response command here.
exit:
    if (err == EMBER_ZCL_NETWORK_COMMISSIONING_ERROR_SUCCESS)
    {
        DeviceLayer::Internal::DeviceControlServer::DeviceControlSvr().EnableNetworkForOperational(networkID);
    }
    response.errorCode = err;
    apCommandHandler->AddResponseData(commandPath, response);
}

void SetDeviceNetworkCommissioningDelegate(DeviceLayer::Internal::DeviceNetworkCommissioningDelegate * delegate)
{
    commissioningDelegate = delegate;
}

DeviceLayer::Internal::DeviceNetworkCommissioningDelegate * GetDeviceNetworkCommissioningDelegate()
{
    return commissioningDelegate;
}

} // namespace NetworkCommissioning
} // namespace Clusters
} // namespace app
} // namespace chip
