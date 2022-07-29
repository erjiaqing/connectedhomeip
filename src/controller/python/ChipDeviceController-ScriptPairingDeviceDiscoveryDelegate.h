/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2013-2017 Nest Labs, Inc.
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

/**
 *    @file
 *      Declaration of DevicePairingDelegate of CHIP Device Controller for Python
 *
 */

#pragma once

#include "ChipDeviceController-ScriptDevicePairingDelegate.h"

#include <controller/CHIPDeviceController.h>

namespace chip {
namespace Controller {

class ScriptPairingDeviceDiscoveryDelegate : public DeviceDiscoveryDelegate
{
public:
    void Setup(NodeId aNodeId, uint32_t aSetupPINCode, CommissioningParameters aParams,
               ScriptDevicePairingDelegate * apPairingDelegate, DeviceCommissioner * apActiveDeviceCommissioner)
    {
        mNodeId                    = aNodeId;
        mSetupPINCode              = aSetupPINCode;
        mParams                    = aParams;
        mpPairingDelegate          = apPairingDelegate;
        mpActiveDeviceCommissioner = apActiveDeviceCommissioner;
    }
    void OnDiscoveredDevice(const Dnssd::DiscoveredNodeData & nodeData);

private:
    ScriptDevicePairingDelegate * mpPairingDelegate;
    DeviceCommissioner * mpActiveDeviceCommissioner = nullptr;

    CommissioningParameters mParams;
    NodeId mNodeId;
    uint32_t mSetupPINCode;
};

} // namespace Controller
} // namespace chip
