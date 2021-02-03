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

/**
 * @file The network provisioning cluster handlers will be implemented in interaction model API
 * TODO: Implement this.
 */

/**
 * @brief Network Provisioning Cluster AddThreadNetwork Command callback
 * @param operationalDataset
 * @param breadcrumb
 * @param timeoutMs
 */

#include <cinttypes>

bool emberAfNetworkProvisioningClusterAddThreadNetworkCallback(uint8_t * OperationalDataset, uint64_t Breadcrumb,
                                                               uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster AddWiFiNetwork Command callback
 * @param ssid
 * @param credentials
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterAddWiFiNetworkCallback(uint8_t * SSID, uint8_t * Credentials, uint64_t Breadcrumb,
                                                             uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster DisableNetwork Command callback
 * @param networkID
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterDisableNetworkCallback(uint8_t * NetworkID, uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster EnableNetwork Command callback
 * @param networkID
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterEnableNetworkCallback(uint8_t * NetworkID, uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster GetLastNetworkProvisioningResult Command callback
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterGetLastNetworkProvisioningResultCallback(uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster RemoveNetwork Command callback
 * @param networkID
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterRemoveNetworkCallback(uint8_t * NetworkID, uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster ScanNetworks Command callback
 * @param ssid
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterScanNetworksCallback(uint8_t * SSID, uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster TestNetwork Command callback
 * @param networkID
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterTestNetworkCallback(uint8_t * NetworkID, uint64_t Breadcrumb, uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster UpdateThreadNetwork Command callback
 * @param operationalDataset
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterUpdateThreadNetworkCallback(uint8_t * OperationalDataset, uint64_t Breadcrumb,
                                                                  uint32_t TimeoutMs)
{
    return false;
}

/**
 * @brief Network Provisioning Cluster UpdateWiFiNetwork Command callback
 * @param ssid
 * @param credentials
 * @param breadcrumb
 * @param timeoutMs
 */

bool emberAfNetworkProvisioningClusterUpdateWiFiNetworkCallback(uint8_t * SSID, uint8_t * Credentials, uint64_t Breadcrumb,
                                                                uint32_t TimeoutMs)
{
    return false;
}
