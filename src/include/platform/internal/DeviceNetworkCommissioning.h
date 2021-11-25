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

    /**
     * @brief
     *   Called to provision Thread credentials in a device
     *
     */
    virtual CHIP_ERROR ConnectToThreadNetwork(ByteSpan threadData) { return CHIP_ERROR_NOT_IMPLEMENTED; };

    class ScanWiFiNetworkCallback
    {
    public:
        /**
         * Indicates the scan is finished, and accepts a list of networks discovered.
         * OnError() must be called in a thread-safe manner with CHIP stack. (e.g. using ScheduleWork or ScheduleLambda)
         */
        virtual void
        OnScanFinished(const Span<app::Clusters::NetworkCommissioning::Structs::WiFiInterfaceScanResult::Type> & network) = 0;

        /**
         * Indicates there is an error during scanning the network. OnError() may be called multiple times.
         * OnError() must be called in a thread-safe manner with CHIP stack. (e.g. using ScheduleWork or ScheduleLambda)
         */
        virtual void OnError(CHIP_ERROR err) = 0;

        virtual ~ScanWiFiNetworkCallback() = default;
    };

    class ScanThreadNetworkCallback
    {
    public:
        /**
         * Indicates the scan is finished, and accepts a list of networks discovered.
         * OnError() must be called in a thread-safe manner with CHIP stack. (e.g. using ScheduleWork or ScheduleLambda)
         */
        virtual void
        OnScanFinished(const Span<app::Clusters::NetworkCommissioning::Structs::ThreadInterfaceScanResult::Type> & network) = 0;

        /**
         * Indicates there is an error during scanning the network. OnError() may be called multiple times.
         * OnError() must be called in a thread-safe manner with CHIP stack. (e.g. using ScheduleWork or ScheduleLambda)
         */
        virtual void OnError(CHIP_ERROR err) = 0;

        virtual ~ScanThreadNetworkCallback() = default;
    };

    /**
     * @brief
     *   Called to initialtiate a WiFi network scan, exactly one of callback->OnError or callback->OnScanFinished must be called
     * within timeout.
     *
     * @param timeout The timeout of the scan, the scan MUST be finished or terminated within the given timeout.
     * @param ssid    The interested SSID, the scanning MAY be restricted to to the given SSID.
     */
    virtual void ScanWiFiNetworks(System::Clock::Milliseconds32 timeout, ByteSpan ssid, ScanWiFiNetworkCallback * callback)
    {
        callback->OnError(CHIP_ERROR_NOT_IMPLEMENTED);
    }

    /**
     * @brief
     *   Called to initialtiate a Thread network scan, exactly one of callback->OnError or callback->OnScanFinished must be called
     * within timeout.
     *
     * @param timeout The timeout of the scan, the scan MUST be finished or terminated within the given timeout.
     */
    virtual void ScanThreadNetworks(System::Clock::Milliseconds32 timeout, ScanThreadNetworkCallback * callback)
    {
        callback->OnError(CHIP_ERROR_NOT_IMPLEMENTED);
    }

    virtual ~DeviceNetworkCommissioningDelegate() {}
};

} // namespace Internal
} // namespace DeviceLayer
} // namespace chip
