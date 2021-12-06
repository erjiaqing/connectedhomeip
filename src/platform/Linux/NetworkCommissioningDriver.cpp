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

#include <lib/support/CodeUtils.h>
#include <lib/support/SafeInt.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/Linux/NetworkCommissioningDriver.h>
#include <platform/Linux/ThreadStackManagerImpl.h>
#include <platform/ThreadStackManager.h>

#include <string>
#include <vector>

using namespace chip;
using namespace chip::Thread;

namespace chip {
namespace DeviceLayer {
namespace NetworkCommissioning {

#if CHIP_DEVICE_CONFIG_ENABLE_WPA
CHIP_ERROR LinuxWiFiDriver::CommitConfiguration()
{
    mRunningNetwork = mStagingNetwork;
    return CHIP_NO_ERROR;
}

CHIP_ERROR LinuxWiFiDriver::RevertConfiguration()
{
    mStagingNetwork = mRunningNetwork;
    return CHIP_NO_ERROR;
}

Status LinuxWiFiDriver::AddOrUpdateNetwork(ByteSpan ssid, ByteSpan credentials)
{
    VerifyOrReturnError(
        mStagingNetwork.ssidLen == 0 ||
            (ssid.size() == mStagingNetwork.ssidLen && memcmp(ssid.data(), mStagingNetwork.ssid, mStagingNetwork.ssidLen) == 0),
        Status::kBoundsExceeded);

    VerifyOrReturnError(credentials.size() <= sizeof(mStagingNetwork.credentials), Status::kOutOfRange);
    memcpy(mStagingNetwork.credentials, credentials.data(), credentials.size());

    using WiFiCredentialsLenType = decltype(mStagingNetwork.credentialsLen);
    VerifyOrReturnError(CanCastTo<WiFiCredentialsLenType>(credentials.size()), Status::kOutOfRange);
    mStagingNetwork.credentialsLen = static_cast<WiFiCredentialsLenType>(credentials.size());

    VerifyOrReturnError(ssid.size() <= sizeof(mStagingNetwork.ssid), Status::kOutOfRange);
    memcpy(mStagingNetwork.ssid, ssid.data(), ssid.size());

    using WiFiSSIDLenType = decltype(mStagingNetwork.ssidLen);
    VerifyOrReturnError(CanCastTo<WiFiSSIDLenType>(ssid.size()), Status::kOutOfRange);
    mStagingNetwork.ssidLen = static_cast<WiFiSSIDLenType>(ssid.size());

    return Status::kSuccess;
}

Status LinuxWiFiDriver::RemoveNetwork(ByteSpan networkId)
{
    VerifyOrReturnError(networkId.size() == mStagingNetwork.ssidLen &&
                            memcmp(networkId.data(), mStagingNetwork.ssid, mStagingNetwork.ssidLen) == 0,
                        Status::kNetworkNotFound);

    mStagingNetwork.ssidLen = 0;
    return Status::kSuccess;
}

Status LinuxWiFiDriver::ReorderNetwork(ByteSpan networkId, uint8_t index)
{
    // We only support one network, so reorder is actually no-op.
    VerifyOrReturnError(networkId.size() == mStagingNetwork.ssidLen &&
                            memcmp(networkId.data(), mStagingNetwork.ssid, mStagingNetwork.ssidLen) == 0,
                        Status::kNetworkNotFound);

    mStagingNetwork.ssidLen = 0;
    return Status::kSuccess;
}

void LinuxWiFiDriver::ConnectNetwork(ByteSpan networkId, ConnectCallback * callback)
{
    CHIP_ERROR err          = CHIP_NO_ERROR;
    Status networkingStatus = Status::kSuccess;

    // We only support one network, so reorder is actually no-op.
    VerifyOrExit(networkId.size() == mStagingNetwork.ssidLen &&
                     memcmp(networkId.data(), mStagingNetwork.ssid, mStagingNetwork.ssidLen) == 0,
                 networkingStatus = Status::kNetworkNotFound);

    ChipLogProgress(NetworkProvisioning, "LinuxNetworkCommissioningDelegate: SSID: %s", networkId.data());

    err = ConnectivityMgrImpl().ProvisionWiFiNetwork(reinterpret_cast<const char *>(mStagingNetwork.ssid),
                                                     reinterpret_cast<const char *>(mStagingNetwork.credentials));

exit:

    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(NetworkProvisioning, "Failed to connect to WiFi network: %s", chip::ErrorStr(err));
        networkingStatus = Status::kUnknownError;
    }

    callback->OnResult(networkingStatus, CharSpan(), 0);
}

void LinuxWiFiDriver::ScanNetworks(ByteSpan ssid, WiFiDriver::ScanCallback * callback)
{
    CHIP_ERROR err = DeviceLayer::ConnectivityMgrImpl().StartWiFiScan(ssid, callback);
    if (err != CHIP_NO_ERROR)
    {
        callback->OnFinished(Status::kUnknownError, CharSpan(), nullptr);
    }
}

size_t LinuxWiFiDriver::WiFiNetworkIterator::Count()
{
    return driver->mStagingNetwork.ssidLen == 0 ? 0 : 1;
}

bool LinuxWiFiDriver::WiFiNetworkIterator::Next(Network & item)
{
    if (exhausted || driver->mStagingNetwork.ssidLen == 0)
    {
        return false;
    }
    memcpy(item.networkID, driver->mStagingNetwork.ssid, driver->mStagingNetwork.ssidLen);
    item.networkIDLen = driver->mStagingNetwork.ssidLen;
    item.connected    = false;
    exhausted         = true;
    return true;
}

#endif

CHIP_ERROR LinuxThreadDriver::CommitConfiguration()
{
    mRunningNetwork = mStagingNetwork;
    return CHIP_NO_ERROR;
}

CHIP_ERROR LinuxThreadDriver::RevertConfiguration()
{
    mStagingNetwork = mRunningNetwork;
    return CHIP_NO_ERROR;
}

Status LinuxThreadDriver::AddOrUpdateNetwork(ByteSpan operationalDataset)
{
    uint8_t extpanid[kSizeExtendedPanId];
    uint8_t newExtpanid[kSizeExtendedPanId];
    Thread::OperationalDataset newDataset;

    newDataset.Init(operationalDataset);
    VerifyOrReturnError(newDataset.IsCommissioned(), Status::kOutOfRange);

    VerifyOrReturnError(!mStagingNetwork.IsCommissioned() || memcmp(extpanid, newExtpanid, kSizeExtendedPanId) == 0,
                        Status::kBoundsExceeded);

    mStagingNetwork = newDataset;
    return Status::kSuccess;
}

Status LinuxThreadDriver::RemoveNetwork(ByteSpan networkId)
{
    uint8_t extpanid[kSizeExtendedPanId];
    if (!mStagingNetwork.IsCommissioned())
    {
        return Status::kNetworkNotFound;
    }
    else if (mStagingNetwork.GetExtendedPanId(extpanid) != CHIP_NO_ERROR)
    {
        return Status::kUnknownError;
    }

    VerifyOrReturnError(networkId.size() == kSizeExtendedPanId && memcmp(networkId.data(), extpanid, kSizeExtendedPanId) == 0,
                        Status::kNetworkNotFound);
    mStagingNetwork.Clear();
    return Status::kSuccess;
}

Status LinuxThreadDriver::ReorderNetwork(ByteSpan networkId, uint8_t index)
{
    uint8_t extpanid[kSizeExtendedPanId];
    if (!mStagingNetwork.IsCommissioned())
    {
        return Status::kNetworkNotFound;
    }
    else if (mStagingNetwork.GetExtendedPanId(extpanid) != CHIP_NO_ERROR)
    {
        return Status::kUnknownError;
    }

    VerifyOrReturnError(networkId.size() == kSizeExtendedPanId && memcmp(networkId.data(), extpanid, kSizeExtendedPanId) == 0,
                        Status::kNetworkNotFound);

    return Status::kSuccess;
}

void LinuxThreadDriver::ConnectNetwork(ByteSpan networkId, ConnectCallback * callback)
{
    NetworkCommissioning::Status status = Status::kSuccess;
    uint8_t extpanid[kSizeExtendedPanId];
    if (!mStagingNetwork.IsCommissioned())
    {
        ExitNow(status = Status::kNetworkNotFound);
    }
    else if (mStagingNetwork.GetExtendedPanId(extpanid) != CHIP_NO_ERROR)
    {
        ExitNow(status = Status::kUnknownError);
    }

    VerifyOrExit((networkId.size() == kSizeExtendedPanId && memcmp(networkId.data(), extpanid, kSizeExtendedPanId) == 0),
                 status = Status::kNetworkNotFound);

    VerifyOrExit(DeviceLayer::ThreadStackMgrImpl().AttachToThreadNetwork(mStagingNetwork.AsByteSpan(), callback) == CHIP_NO_ERROR,
                 status = Status::kUnknownError);

exit:
    if (status != Status::kSuccess)
    {
        callback->OnResult(status, CharSpan(), 0);
    }
}

void LinuxThreadDriver::ScanNetworks(ThreadDriver::ScanCallback * callback)
{
    CHIP_ERROR err = DeviceLayer::ThreadStackMgrImpl().StartThreadScan(callback);
    if (err != CHIP_NO_ERROR)
    {
        callback->OnFinished(Status::kUnknownError, CharSpan(), nullptr);
    }
}

size_t LinuxThreadDriver::ThreadNetworkIterator::Count()
{
    return driver->mStagingNetwork.IsCommissioned() ? 1 : 0;
}

bool LinuxThreadDriver::ThreadNetworkIterator::Next(Network & item)
{
    if (exhausted || !driver->mStagingNetwork.IsCommissioned())
    {
        return false;
    }
    uint8_t extpanid[kSizeExtendedPanId];
    VerifyOrReturnError(driver->mStagingNetwork.GetExtendedPanId(extpanid) == CHIP_NO_ERROR, false);
    memcpy(item.networkID, extpanid, kSizeExtendedPanId);
    item.networkIDLen = kSizeExtendedPanId;
    item.connected    = false;
    exhausted         = true;
    return true;
}

} // namespace NetworkCommissioning
} // namespace DeviceLayer
} // namespace chip
