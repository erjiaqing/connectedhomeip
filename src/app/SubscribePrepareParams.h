/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

#include <app/util/basic-types.h>
#include <core/CHIPCore.h>
#include <core/CHIPTLV.h>
#include <app/EventPathParams.h>
#include <app/AttributePathParams.h>
#include <messaging/ExchangeMgr.h>

namespace chip {
namespace app {
struct SubscribePrepareParams
{
    NodeId mNodeId = 0;
    FabricIndex mFabricIndex = 0;
    SecureSessionHandle * mpSecureSession       = nullptr;
    EventPathParams * mpEventPathParamsList = nullptr;
    size_t mEventPathParamsListSize   = 0;
    AttributePathParams * mpAttributePathParamsList  = nullptr;
    size_t mAttributePathParamsListSize = 0;
    EventNumber mEventNumber = 0;
    uint16_t mMinIntervalSeconds = 0;
    uint16_t mMaxIntervalSeconds = 0;
};
} // namespace app
} // namespace chip
