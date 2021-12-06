/*
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
 *    @file
 *      This file defines the CHIP Device Network Provisioning object.
 *
 */

#pragma once

#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPSafeCasts.h>
#include <lib/support/ThreadOperationalDataset.h>
#include <platform/internal/DeviceNetworkInfo.h>

namespace chip {
namespace DeviceLayer {
/**
 * We are using a namespace here, for most use cases, this namespace will be used by `using DeviceLayer::NetworkCommissioning`, but
 * this still worth a dedicated namespace since:
 *
 * - The BaseDriver / WirelessDriver is not expected to be implemented directly by users, the only occurrance is in the network
 *   commissioning cluster.
 * - We can safely name the drivers as WiFiDriver / ThreadDriver, it should not be ambiguous for most cases
 * - We can safely name the Status enum to Status, and some other structs -- if we are using using, then we should in the context of
 *   writing something dedicated to network commissioning, then a single word Status should be clear enough.
 */
namespace NetworkCommissioning {

constexpr size_t kMaxNetworkIDLen = 32;

// TODO: This is exactly the same as the one in GroupDataProvider, this could be moved to src/lib/support
template <typename T>
class Iterator
{
public:
    virtual ~Iterator() = default;
    /**
     *  @retval The number of entries in total that will be iterated.
     */
    virtual size_t Count() = 0;
    /**
     *   @param[out] item  Value associated with the next element in the iteration.
     *  @retval true if the next entry is successfully retrieved.
     *  @retval false if no more entries can be found.
     */
    virtual bool Next(T & item) = 0;
    /**
     * Release the memory allocated by this iterator.
     * Must be called before the pointer goes out of scope.
     */
    virtual void Release() = 0;

protected:
    Iterator() = default;
};

/**
 * The content should match the one in zap_generated/cluster-objects.h
 */
enum class Status : uint8_t
{
    kSuccess                = 0x00,
    kOutOfRange             = 0x01,
    kBoundsExceeded         = 0x02,
    kNetworkIDNotFound      = 0x03,
    kDuplicateNetworkID     = 0x04,
    kNetworkNotFound        = 0x05,
    kRegulatoryError        = 0x06,
    kAuthFailure            = 0x07,
    kUnsupportedSecurity    = 0x08,
    kOtherConnectionFailure = 0x09,
    kIPV6Failed             = 0x0A,
    kIPBindFailed           = 0x0B,
    kUnknownError           = 0x0C,
};

// The following structs follows the generated cluster object structs.
struct Network
{
    uint8_t networkID[kMaxNetworkIDLen];
    uint8_t networkIDLen;
    bool connected;
};

struct WiFiScanResponse
{
public:
    uint8_t security;
    uint8_t ssid[DeviceLayer::Internal::kMaxWiFiSSIDLength];
    uint8_t ssidLen;
    uint8_t bssid[6];
    uint8_t channel;
    uint32_t wiFiBand;
    int8_t rssi;
};

struct ThreadScanResponse
{
    uint64_t panId;
    uint64_t extendedPanId;
    char networkName[16];
    uint8_t networkNameLen;
    uint8_t ssidLen;
    uint16_t channel;
    uint8_t version;
    uint64_t extendedAddress;
    int8_t rssi;
    uint8_t lqi;
};

using NetworkIterator            = Iterator<Network>;
using WiFiScanResponseIterator   = Iterator<WiFiScanResponse>;
using ThreadScanResponseIterator = Iterator<ThreadScanResponse>;

// BaseDriver and WirelessDriver are the common interfaces for a network driver, platform drivers should not implement this
// directly, instead, users are expected to implement WiFiDriver, ThreadDriver and EthernetDriver.
namespace Internal {
class BaseDriver
{
public:
    /**
     * @brief Returns maximum number of network configs can be added to the driver.
     */
    virtual uint8_t GetMaxNetworks() = 0;
    /**
     * @brief Returns an iterator for reading the networks, the user will always call NetworkIterator::Release.
     */
    virtual NetworkIterator * GetNetworks() = 0;

    /**
     * @brief Sets the status of the interface, this is an optional feature of a network driver.
     */
    virtual CHIP_ERROR SetEnabled(bool enabled) { return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE; }
    /**
     * @brief Returns the status of the interface, this is an optional feature of a network driver the driver will be enabled by
     * default.
     */
    virtual bool GetEnabled() { return true; };

    virtual ~BaseDriver() = default;
};

class WirelessDriver : public Internal::BaseDriver
{
public:
    class ConnectCallback
    {
    public:
        virtual void OnResult(Status commissioningError, CharSpan debugText, int32_t connectStatus) = 0;

        virtual ~ConnectCallback() = default;
    };

    /**
     * @brief Persist the network configuration. This function is expected to be called when CommissioningComplete event is fired.
     */
    virtual CHIP_ERROR CommitConfiguration() = 0;

    /**
     * @brief Revert the network configuration to the last committed one. This function is expected to be called when failsafe
     * timeout reached.
     */
    virtual CHIP_ERROR RevertConfiguration() = 0;

    virtual uint8_t GetScanNetworkTimeoutSeconds()    = 0;
    virtual uint8_t GetConnectNetworkTimeoutSeconds() = 0;

    virtual Status RemoveNetwork(ByteSpan networkId)                 = 0;
    virtual Status ReorderNetwork(ByteSpan networkId, uint8_t index) = 0;

    /**
     * @brief Called to initiate a network join. callback->OnResult must be called, on both success and error. Callback can be
     * called inside ConnectNetwork.
     */
    virtual void ConnectNetwork(ByteSpan networkId, ConnectCallback * callback) = 0;
};
} // namespace Internal

class WiFiDriver : public Internal::WirelessDriver
{
public:
    class ScanCallback
    {
    public:
        /**
         * Indicates the scan is finished, and accepts a iterator of networks discovered.
         * - Iterator::Release must be called inside this function.
         * - networks can be nullptr when no networks discovered, or error occurred during scanning the network.
         * OnFinished() must be called in a thread-safe manner with CHIP stack. (e.g. using ScheduleWork or ScheduleLambda)
         */
        virtual void OnFinished(Status status, CharSpan debugText, WiFiScanResponseIterator * networks) = 0;

        virtual ~ScanCallback() = default;
    };

    virtual Status AddOrUpdateNetwork(ByteSpan ssid, ByteSpan credentials) = 0;

    /**
     * @brief Called to initiate a WiFi network scan. callback->OnFinished must be called, on both success and error. Callback can
     * be called inside ScanNetworks.
     *
     * @param ssid    The interested SSID, the scanning MAY be restricted to to the given SSID.
     */
    virtual void ScanNetworks(ByteSpan ssid, ScanCallback * callback) = 0;

    virtual ~WiFiDriver() = default;
};

class ThreadDriver : public Internal::WirelessDriver
{
public:
    class ScanCallback
    {
    public:
        /**
         * Indicates the scan is finished, and accepts a iterator of networks discovered.
         * - Iterator::Release must be called inside this function.
         * - networks can be nullptr, when no networks discovered, or error occurred during scanning the network.
         * OnFinished() must be called in a thread-safe manner with CHIP stack. (e.g. using ScheduleWork or ScheduleLambda)
         */
        virtual void OnFinished(Status err, CharSpan debugText, ThreadScanResponseIterator * networks) = 0;

        virtual ~ScanCallback() = default;
    };

    virtual Status AddOrUpdateNetwork(ByteSpan operationalDataset) = 0;

    /**
     * @brief Called to initiate a Thread network scan. callback->OnFinished must be called, on both success and error. Callback can
     * be called inside ScanNetworks.
     */
    virtual void ScanNetworks(ScanCallback * callback) = 0;

    virtual ~ThreadDriver() = default;
};

class EthernetDriver : public Internal::BaseDriver
{
    // Ethernet driver does not have any special operations.
};

} // namespace NetworkCommissioning
} // namespace DeviceLayer
} // namespace chip
