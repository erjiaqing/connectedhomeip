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

/**
 *    @file
 *     This file defines read handler for a CHIP Interaction Data model
 *
 */

#pragma once

#include <app/ClusterInfo.h>
#include <app/ConcreteAttributePath.h>
#include <app/EventManagement.h>
#include <app/InteractionModelDelegate.h>
#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPTLVDebug.hpp>
#include <lib/support/CodeUtils.h>
#include <lib/support/DLLUtil.h>
#include <lib/support/logging/CHIPLogging.h>
#include <messaging/ExchangeContext.h>
#include <messaging/ExchangeMgr.h>
#include <messaging/Flags.h>
#include <protocols/Protocols.h>
#include <system/SystemPacketBuffer.h>

namespace chip {
namespace app {

/**
 * PathIterator is used for iterate over a chain of cluster info.
 * The PathIterator is copiable, however, the given cluster info must be valid when calling proceed.
 *
 * PathIterator will expand attribute paths with wildcards.
 *
 * The typical use of PathIterator may look like:
 * ConcreteAttributePath path;
 * for (PathIterator iterator(clusterInfo); iterator.Get(path); iterator.Proceed()) {...}
 */
class PathIterator
{
public:
    PathIterator(ClusterInfo * aClusterInfo) { Reset(aClusterInfo); }
    // Init initialized / resets the state of the path iterator.
    void Reset(ClusterInfo * aClusterInfo);
    /**
     * Proceed the iterator to the next attribute path in the given cluster info.
     */
    bool Proceed();
    /**
     * Fills the aPath with the path the iterator current points to.
     * Return false if the iterator is not pointing a valid path (i.e. it has exhausted the cluster info).
     */
    bool Get(ConcreteAttributePath & aPath)
    {
        aPath = mOutputPath;
        return mOutputPath.mEndpointId != ConcreteAttributePath::kInvalidEndpointId;
    }
    bool Valid() { return mpClusterInfo != nullptr; }

private:
    ClusterInfo * mpClusterInfo;

    uint16_t mEndpointIndex;
    uint8_t mClusterIndex;
    uint16_t mAttributeIndex;

    ConcreteAttributePath mOutputPath = ConcreteAttributePath();

    void PrepareEndpointIndexRange(const Optional<EndpointId> & aEndpointId, uint16_t * aBeginEndpointIndex,
                                   uint16_t * aEndEndpointIndex);
    void PrepareClusterIndexRange(EndpointId aEndpointId, const Optional<ClusterId> & aClusterId, uint8_t * aBeginClusterIndex,
                                  uint8_t * aEndClusterIndex);
    void PrepareAttributeIndexRange(EndpointId aEndpointId, ClusterId aClusterId, const Optional<AttributeId> & aAttributeId,
                                    uint16_t * aBeginAttributeIndex, uint16_t * aEndAttributeIndex);
};
} // namespace app
} // namespace chip
