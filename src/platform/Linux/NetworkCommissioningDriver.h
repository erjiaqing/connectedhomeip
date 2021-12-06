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

#pragma once

#include <platform/NetworkCommissioning.h>
#include <vector>

namespace chip {
namespace DeviceLayer {
namespace NetworkCommissioning {

template <typename T>
class LinuxScanResponseIterator : public Iterator<T>
{
public:
    LinuxScanResponseIterator(std::vector<T> * apScanResponse) : mpScanResponse(apScanResponse) {}
    size_t Count() override { return mpScanResponse != nullptr ? mpScanResponse->size() : 0; }
    bool Next(T & item) override
    {
        if (mpScanResponse == nullptr || currentIterating >= mpScanResponse->size())
        {
            return false;
        }
        item = (*mpScanResponse)[currentIterating];
        currentIterating++;
        return true;
    }
    void Release() override
    { /* nothing to do, we don't hold the ownership of the vector. */
    }

private:
    size_t currentIterating = 0;
    std::vector<T> * mpScanResponse;
};

#if CHIP_DEVICE_CONFIG_ENABLE_WPA
class LinuxWiFiDriver final : public WiFiDriver
{
public:
    class WiFiNetworkIterator final : public NetworkIterator
    {
    public:
        WiFiNetworkIterator(LinuxWiFiDriver * aDriver) : driver(aDriver) {}
        size_t Count() override;
        bool Next(Network & item) override;
        void Release() override
        { /* Do nothing, since we don't hold any resources. */
        }
        ~WiFiNetworkIterator() = default;

    private:
        LinuxWiFiDriver * driver;
        bool exhausted = false;
    };

    struct WiFiNetwork
    {
        uint8_t ssid[DeviceLayer::Internal::kMaxWiFiSSIDLength];
        uint8_t ssidLen = 0;
        uint8_t credentials[DeviceLayer::Internal::kMaxWiFiKeyLength];
        uint8_t credentialsLen = 0;
    };

    struct LinuxWiFiScanResponseIterator;

    // BaseDriver
    NetworkIterator * GetNetworks() override
    {
        mWiFiIterator = WiFiNetworkIterator(this);
        return &mWiFiIterator;
    }

    // WirelessDriver
    uint8_t GetMaxNetworks() override { return 1; }
    uint8_t GetScanNetworkTimeoutSeconds() override { return 10; }
    uint8_t GetConnectNetworkTimeoutSeconds() override { return 20; }

    CHIP_ERROR CommitConfiguration() override;
    CHIP_ERROR RevertConfiguration() override;

    Status RemoveNetwork(ByteSpan networkId) override;
    Status ReorderNetwork(ByteSpan networkId, uint8_t index) override;
    void ConnectNetwork(ByteSpan networkId, ConnectCallback * callback) override;

    // WiFiDriver
    Status AddOrUpdateNetwork(ByteSpan ssid, ByteSpan credentials) override;
    void ScanNetworks(ByteSpan ssid, ScanCallback * callback) override;

private:
    friend WiFiNetworkIterator;
    friend LinuxWiFiScanResponseIterator;

    WiFiNetworkIterator mWiFiIterator = WiFiNetworkIterator(this);
    WiFiNetwork mRunningNetwork;
    WiFiNetwork mStagingNetwork;
};
#endif

class LinuxThreadDriver final : public ThreadDriver
{
    class ThreadNetworkIterator final : public NetworkIterator
    {
    public:
        ThreadNetworkIterator(LinuxThreadDriver * aDriver) : driver(aDriver) {}
        size_t Count() override;
        bool Next(Network & item) override;
        void Release() override
        { /* Do nothing, since we don't hold any resources. */
        }
        ~ThreadNetworkIterator() = default;

    private:
        LinuxThreadDriver * driver;
        bool exhausted = false;
    };

    // BaseDriver
    NetworkIterator * GetNetworks() override
    {
        mThreadIterator = ThreadNetworkIterator(this);
        return &mThreadIterator;
    }

    // WirelessDriver
    uint8_t GetMaxNetworks() override { return 1; }
    uint8_t GetScanNetworkTimeoutSeconds() override { return 10; }
    uint8_t GetConnectNetworkTimeoutSeconds() override { return 20; }

    CHIP_ERROR CommitConfiguration() override;
    CHIP_ERROR RevertConfiguration() override;

    Status RemoveNetwork(ByteSpan networkId) override;
    Status ReorderNetwork(ByteSpan networkId, uint8_t index) override;
    void ConnectNetwork(ByteSpan networkId, ConnectCallback * callback) override;

    // ThreadDriver
    Status AddOrUpdateNetwork(ByteSpan operationalDataset) override;
    void ScanNetworks(ScanCallback * callback) override;

private:
    friend ThreadNetworkIterator;

    ThreadNetworkIterator mThreadIterator = ThreadNetworkIterator(this);
    Thread::OperationalDataset mRunningNetwork;
    Thread::OperationalDataset mStagingNetwork;
};

} // namespace NetworkCommissioning
} // namespace DeviceLayer
} // namespace chip
