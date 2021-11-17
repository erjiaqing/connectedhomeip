/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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
 *    @file
 *      This file defines the CHIP Device Network Provisioning object.
 *
 */

#pragma once

#include <app-common/zap-generated/cluster-objects.h>
#include <lib/core/CHIPCore.h>
#include <platform/internal/DeviceNetworkInfo.h>

namespace chip {
namespace DeviceLayer {
namespace Internal {

class DLL_EXPORT DeviceNetworkCommissioningDelegate
{
public:
    /**
     * @brief
     *   Called to provision WiFi credentials in a device
     *
     * @param ssid WiFi SSID
     * @param passwd WiFi password
     */
    virtual CHIP_ERROR ProvisionWiFi(const char * ssid, const char * passwd) = 0;

    class EnableNetworkCallback
    {
    public:
        virtual void OnError(CHIP_ERROR err) = 0;
        virtual void OnConnected()           = 0;

        virtual ~EnableNetworkCallback() = default;
    };

    /**
     * @brief
     *   Called to provision Thread credentials in a device
     *
     */
    virtual CHIP_ERROR ConnectToThreadNetwork(ByteSpan threadData) { return CHIP_ERROR_NOT_IMPLEMENTED; };

    /**
     * @brief
     *   Called to set the current connected WiFi network.
     *
     * @param timeout The timeout of the network commissioning
     * @param ssid WiFi SSID
     * @param passwd WiFi password
     */
    virtual CHIP_ERROR ConnectToWiFiNetwork(System::Clock::Milliseconds32 timeout, ByteSpan ssid, ByteSpan credentials,
                                            EnableNetworkCallback * callback)
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    };

    class ScanNetworkCallback
    {
    public:
        virtual void
        OnWiFiNetworkDiscovered(const app::Clusters::NetworkCommissioning::Structs::WiFiInterfaceScanResult::Type & network) = 0;
        virtual void OnThreadNetworkDiscovered(
            const app::Clusters::NetworkCommissioning::Structs::ThreadInterfaceScanResult::Type & network) = 0;
        virtual void OnError(CHIP_ERROR err)                                                               = 0;
        virtual void OnDone()                                                                              = 0;

        virtual ~ScanNetworkCallback() = default;
    };

    /**
     * @brief
     *   Called to initialtiate a network scan
     *
     * @param timeout The timeout of the scan, the scan MUST be finished or terminated within the given timeout.
     * @param ssid    The interested SSID, the scanning MAY be restricted to to the given SSID.
     */
    virtual void ScanNetworks(System::Clock::Milliseconds32 timeout, ScanNetworkCallback * callback)
    {
        callback->OnError(CHIP_ERROR_NOT_IMPLEMENTED);
        callback->OnDone();
    }

    virtual ~DeviceNetworkCommissioningDelegate() {}
};

} // namespace Internal
} // namespace DeviceLayer
} // namespace chip
