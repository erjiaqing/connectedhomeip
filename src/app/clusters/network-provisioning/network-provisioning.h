#pragma once

#include <core/CHIPCore.h>
#include <platform/CHIPDeviceLayer.h>

namespace chip {
namespace app {
namespace cluster {
namespace NetworkProvisioning {

void SetDeviceNetworkProvisioningDelegate(DeviceLayer::Internal::DeviceNetworkProvisioningDelegate * delegate);

void HandleAddThreadNetworkCommandReceived(app::Command * apCommandObj, chip::EndpointId aEndpoint,
                                           const uint8_t * operationalDataset, const uint32_t operationalDatasetLen,
                                           uint64_t Breadcrumb, uint32_t TimeoutMs);
void HandleAddWiFiNetworkCommandReceived(app::Command * apCommandObj, chip::EndpointId aEndpoint, const uint8_t * ssid,
                                         const uint32_t ssidLen, const uint8_t * credentials, const uint32_t credentialsLen,
                                         uint64_t Breadcrumb, uint32_t TimeoutMs);
void HandleEnableNetworkCommandReceived(app::Command * apCommandObj, chip::EndpointId aEndpoint, const uint8_t * networkId,
                                        const uint32_t networkIdLen, uint64_t Breadcrumb, uint32_t TimeoutMs);

} // namespace NetworkProvisioning
} // namespace cluster
} // namespace app
} // namespace chip
