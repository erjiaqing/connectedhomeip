/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
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

#include "Stack.h"

#include <controller/python/ChipDeviceController-ScriptDevicePairingDelegate.h>
#include <controller/python/ChipDeviceController-ScriptPairingDeviceDiscoveryDelegate.h>
#include <credentials/GroupDataProviderImpl.h>
#include <credentials/PersistentStorageOpCertStore.h>

// TODO: This files declares all global variables used by Python script API, we should consider remove them or wrap them with some
// non-global objects.

namespace chip {
namespace Python {
namespace Globals {
extern Credentials::PersistentStorageOpCertStore sPersistentStorageOpCertStore;
extern Credentials::GroupDataProviderImpl sGroupDataProvider;
extern Controller::ScriptDevicePairingDelegate sPairingDelegate;
extern Controller::ScriptPairingDeviceDiscoveryDelegate sPairingDeviceDiscoveryDelegate;
} // namespace Globals
} // namespace Python
} // namespace chip
