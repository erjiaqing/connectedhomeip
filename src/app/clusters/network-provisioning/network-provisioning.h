#pragma once

#include <platform/internal/DeviceNetworkProvisioning.h>

namespace chip {
namespace app {
namespace cluster {
namespace NetworkProvisioning {
void SetDeviceNetworkProvisioningDelegate(DeviceLayer::Internal::DeviceNetworkProvisioningDelegate * delegate);
}
} // namespace cluster
} // namespace app
} // namespace chip
