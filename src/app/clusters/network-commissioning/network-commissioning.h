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

#pragma once

#include <app-common/zap-generated/enums.h>
#include <app/CommandHandler.h>
#include <lib/core/CHIPCore.h>
#include <lib/support/ThreadOperationalDataset.h>
#include <platform/internal/DeviceNetworkProvisioning.h>

#ifndef CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS
#define CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS 4
#endif // CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS

#ifndef CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_WIFI_SCAN_RESULTS
#define CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_WIFI_SCAN_RESULTS 4
#endif // CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS

#ifndef CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_THREAD_SCAN_RESULTS
#define CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_THREAD_SCAN_RESULTS 4
#endif // CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS

namespace chip {
namespace app {
namespace Clusters {
namespace NetworkCommissioning {

constexpr uint8_t kMaxNetworkIDLen       = 32;
constexpr uint8_t kMaxThreadDatasetLen   = 254; // As defined in Thread spec.
constexpr uint8_t kMaxWiFiSSIDLen        = 32;
constexpr uint8_t kMaxWiFiCredentialsLen = 64;
constexpr uint8_t kMaxNetworks           = CHIP_CLUSTER_NETWORK_COMMISSIONING_MAX_NETWORKS;

enum class NetworkType : uint8_t
{
    kUndefined = 0,
    kWiFi      = 1,
    kThread    = 2,
    kEthernet  = 3,
};

struct ThreadNetworkInfo
{
    uint8_t mDataset[kMaxThreadDatasetLen];
    uint8_t mDatasetLen;
};

struct WiFiScanInfo
{
    uint8_t security;
    uint8_t ssid[kMaxWiFiSSIDLen + 1];
    uint8_t ssidLen;
    uint8_t bssid[kMaxWiFiSSIDLen + 1];
    uint8_t bssidLen;
    uint8_t channel;
    uint32_t frequencyBand;
};

struct WiFiNetworkInfo
{
    uint8_t mSSID[kMaxWiFiSSIDLen + 1];
    uint8_t mSSIDLen;
    uint8_t mCredentials[kMaxWiFiCredentialsLen];
    uint8_t mCredentialsLen;
};

struct NetworkInfo
{
    uint8_t mNetworkID[kMaxNetworkIDLen];
    uint8_t mNetworkIDLen;
    uint8_t mEnabled;
    NetworkType mNetworkType;
    union NetworkData
    {
        Thread::OperationalDataset mThread;
        WiFiNetworkInfo mWiFi;
    } mData;
};

void OnAddThreadNetworkCommandCallbackInternal(app::CommandHandler *, const app::ConcreteCommandPath & commandPath,
                                               ByteSpan operationalDataset, uint64_t breadcrumb, uint32_t timeoutMs);
void OnAddWiFiNetworkCommandCallbackInternal(app::CommandHandler *, const app::ConcreteCommandPath & commandPath, ByteSpan ssid,
                                             ByteSpan credentials, uint64_t breadcrumb, uint32_t timeoutMs);
void OnEnableNetworkCommandCallbackInternal(app::CommandHandler *, const app::ConcreteCommandPath & commandPath, ByteSpan networkID,
                                            uint64_t breadcrumb, uint32_t timeoutMs);

void SetDeviceNetworkCommissioningDelegate(DeviceLayer::Internal::DeviceNetworkCommissioningDelegate * delegate);

namespace Internal {
DeviceLayer::Internal::DeviceNetworkCommissioningDelegate * GetDeviceNetworkCommissioningDelegate();
}

} // namespace NetworkCommissioning

} // namespace Clusters
} // namespace app
} // namespace chip
