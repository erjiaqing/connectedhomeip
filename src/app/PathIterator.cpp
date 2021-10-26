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

#include <app/PathIterator.h>

#include <app-common/zap-generated/att-storage.h>
#include <app/ClusterInfo.h>
#include <app/ConcreteAttributePath.h>
#include <app/EventManagement.h>
#include <app/InteractionModelDelegate.h>
#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPTLVDebug.hpp>
#include <lib/support/CodeUtils.h>
#include <lib/support/DLLUtil.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;

// TODO: Here we use forward declaration for these symbols used, there should be some reorganize for code in app/util so they can be
// used with generated files or some mock files.
typedef uint8_t EmberAfClusterMask;
#define CLUSTER_MASK_SERVER (0x40)

extern uint16_t emberAfEndpointCount(void);
extern uint16_t emberAfIndexFromEndpoint(EndpointId endpoint);
extern uint8_t emberAfClusterCount(EndpointId endpoint, bool server);
extern uint16_t emberAfGetServerAttributeCount(chip::EndpointId endpoint, chip::ClusterId cluster);
extern uint16_t emberAfGetServerAttributeIndexByAttributeId(chip::EndpointId endpoint, chip::ClusterId cluster,
                                                            chip::AttributeId attributeId);
extern chip::EndpointId emberAfEndpointFromIndex(uint16_t index);
extern Optional<ClusterId> emberAfGetNthClusterId(chip::EndpointId endpoint, uint8_t n, bool server);
extern Optional<AttributeId> emberAfGetServerAttributeIdByIndex(chip::EndpointId endpoint, chip::ClusterId cluster,
                                                                uint16_t attributeIndex);
extern uint8_t emberAfClusterIndex(EndpointId endpoint, ClusterId clusterId, EmberAfClusterMask mask);

namespace chip {
namespace app {

void PathIterator::Reset(ClusterInfo * aClusterInfo)
{
    mpClusterInfo = aClusterInfo;

    // Reset iterator state
    mEndpointIndex  = UINT16_MAX;
    mClusterIndex   = UINT8_MAX;
    mAttributeIndex = UINT16_MAX;

    Proceed();
}

void PathIterator::PrepareEndpointIndexRange(const ClusterInfo & aClusterInfo, uint16_t * aBeginEndpointIndex,
                                             uint16_t * aEndEndpointIndex)
{
    if (!aClusterInfo.HasValidEndpointId())
    {
        *aBeginEndpointIndex = 0;
        *aEndEndpointIndex   = emberAfEndpointCount();
    }
    else
    {
        *aBeginEndpointIndex = emberAfIndexFromEndpoint(aClusterInfo.mEndpointId);
        // If the given cluster id does not exist on the given endpoint, it will return uint16(0xFFFF), then endEndpointIndex
        // will be 0, means we should iterate a null endppint set (skip it).
        *aEndEndpointIndex = static_cast<uint16_t>(*aBeginEndpointIndex + 1);
    }
}

void PathIterator::PrepareClusterIndexRange(const ClusterInfo & aClusterInfo, EndpointId aEndpointId, uint8_t * aBeginClusterIndex,
                                            uint8_t * aEndClusterIndex)
{
    if (!aClusterInfo.HasValidClusterId())
    {
        *aBeginClusterIndex = 0;
        *aEndClusterIndex   = emberAfClusterCount(aEndpointId, true /* server */);
    }
    else
    {
        *aBeginClusterIndex = emberAfClusterIndex(aEndpointId, aClusterInfo.mClusterId, CLUSTER_MASK_SERVER);
        // If the given cluster id does not exist on the given endpoint, it will return uint8(0xFF), then endClusterIndex
        // will be 0, means we should i
        *aEndClusterIndex = static_cast<uint8_t>(*aBeginClusterIndex + 1);
    }
}

void PathIterator::PrepareAttributeIndexRange(const ClusterInfo & aClusterInfo, EndpointId aEndpointId, ClusterId aClusterId,
                                              uint16_t * aBeginAttributeIndex, uint16_t * aEndAttributeIndex)
{
    if (!aClusterInfo.HasValidAttributeId())
    {
        *aBeginAttributeIndex = 0;
        *aEndAttributeIndex   = emberAfGetServerAttributeCount(aEndpointId, aClusterId);
    }
    else
    {
        *aBeginAttributeIndex = emberAfGetServerAttributeIndexByAttributeId(aEndpointId, aClusterId, aClusterInfo.mFieldId);
        // If the given attribute id does not exist on the given endpoint, it will return uint16(0xFFFF), then endAttributeIndex
        // will be 0
        *aEndAttributeIndex = static_cast<uint16_t>(*aBeginAttributeIndex + 1);
    }
}

bool PathIterator::Proceed()
{
    for (; mpClusterInfo != nullptr; (mpClusterInfo = mpClusterInfo->mpNext, mEndpointIndex = UINT16_MAX))
    {
        // Special case: If this is a concrete path, we just return its value as-is.
        if (!mpClusterInfo->HasWildcard())
        {
            mOutputPath.mEndpointId  = mpClusterInfo->mEndpointId;
            mOutputPath.mClusterId   = mpClusterInfo->mClusterId;
            mOutputPath.mAttributeId = mpClusterInfo->mFieldId;

            // Prepare for next iteration
            (mpClusterInfo = mpClusterInfo->mpNext, mEndpointIndex = UINT16_MAX);
            return true;
        }
        uint16_t beginEndpointIndex, endEndpointIndex;
        PrepareEndpointIndexRange(*mpClusterInfo, &beginEndpointIndex, &endEndpointIndex);

        if (mEndpointIndex == UINT16_MAX)
        {
            // If we have not started iterating over the endpoints yet.
            mEndpointIndex = beginEndpointIndex;
            mClusterIndex  = UINT8_MAX;
        }

        for (; mEndpointIndex < endEndpointIndex; (mEndpointIndex++, mClusterIndex = UINT8_MAX, mAttributeIndex = UINT16_MAX))
        {
            EndpointId endpointId = emberAfEndpointFromIndex(mEndpointIndex);
            uint8_t beginClusterIndex, endClusterIndex;
            PrepareClusterIndexRange(*mpClusterInfo, endpointId, &beginClusterIndex, &endClusterIndex);

            if (mClusterIndex == UINT8_MAX)
            {
                // If we have not started iterating over the clusters yet.
                mClusterIndex   = beginClusterIndex;
                mAttributeIndex = UINT16_MAX;
            }

            for (; mClusterIndex < endClusterIndex; (mClusterIndex++, mAttributeIndex = UINT16_MAX))
            {
                // emberAfGetNthClusterId must return a valid cluster id here since we have verified the mClusterIndex does
                // not exceed the endAttributeIndex.
                ClusterId clusterId = emberAfGetNthClusterId(endpointId, mClusterIndex, true /* server */).Value();
                uint16_t beginAttributeIndex, endAttributeIndex;
                PrepareAttributeIndexRange(*mpClusterInfo, endpointId, clusterId, &beginAttributeIndex, &endAttributeIndex);
                if (mAttributeIndex == UINT16_MAX)
                {
                    // If we have not started iterating over the attributes yet.
                    mAttributeIndex = beginAttributeIndex;
                }

                if (mAttributeIndex < endAttributeIndex)
                {
                    // GetServerAttributeIdByIdex must return a valid attribute here since we have verified the mAttributeindex does
                    // not exceed the endAttributeIndex.
                    mOutputPath.mAttributeId = emberAfGetServerAttributeIdByIndex(endpointId, clusterId, mAttributeIndex).Value();
                    mOutputPath.mClusterId   = clusterId;
                    mOutputPath.mEndpointId  = endpointId;
                    mAttributeIndex++;
                    // We found a valid attribute path, now return and increase the attribute index for next iteration.
                    // Return true will skip the increment of mClusterIndex, mEndpointIndex and mpClusterInfo.
                    return true;
                }
                // We have exhausted all attributes of this cluster, continue iterating over attributes of next cluster.
            }
            // We have exhausted all clusters of this endpoint, continue iterating over clusters of next endpoint.
        }
        // We have exhausted all endpoints in this cluster info, continue iterating over next cluster info item.
    }

    // Reset to default, invalid value.
    mOutputPath = ConcreteAttributePath();
    return false;
}
} // namespace app
} // namespace chip
