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

typedef uint8_t EmberAfClusterMask;
#define CLUSTER_MASK_SERVER (0x40)

extern uint16_t emberAfEndpointCount(void);
extern uint16_t emberAfIndexFromEndpoint(EndpointId endpoint);
extern uint8_t emberAfClusterCount(EndpointId endpoint, bool server);
extern uint16_t emberAfGetServerAttributeCount(chip::EndpointId endpoint, chip::ClusterId cluster);
extern uint16_t emberAfGetServerAttributeIndexByAttributeId(chip::EndpointId endpoint, chip::ClusterId cluster,
                                                            chip::AttributeId attributeId);
extern chip::EndpointId emberAfEndpointFromIndex(uint16_t index);
extern chip::ClusterId emberAfGetNthClusterId(chip::EndpointId endpoint, uint8_t n, bool server);
extern chip::AttributeId emberAfGetServerAttributeIdByIndex(chip::EndpointId endpoint, chip::ClusterId cluster,
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

void PathIterator::PrepareEndpointIndexRange(const Optional<EndpointId> & aEndpointId, uint16_t * aBeginEndpointIndex,
                                             uint16_t * aEndEndpointIndex)
{
    if (!aEndpointId.HasValue())
    {
        *aBeginEndpointIndex = 0;
        *aEndEndpointIndex   = emberAfEndpointCount();
    }
    else
    {
        *aBeginEndpointIndex = emberAfIndexFromEndpoint(aEndpointId.Value());
        *aEndEndpointIndex   = *aBeginEndpointIndex + 1;
    }
}

void PathIterator::PrepareClusterIndexRange(EndpointId aEndpointId, const Optional<ClusterId> & aClusterId,
                                            uint8_t * aBeginClusterIndex, uint8_t * aEndClusterIndex)
{
    if (!aClusterId.HasValue())
    {
        *aBeginClusterIndex = 0;
        *aEndClusterIndex   = emberAfClusterCount(aEndpointId, true /* server */);
    }
    else
    {
        *aBeginClusterIndex = emberAfClusterIndex(aEndpointId, aClusterId.Value(), CLUSTER_MASK_SERVER);
        // If the given cluster id does not exists on the given endpoint, it will return uint8(0xFF), then endClusterIndex
        // will be 0
        *aEndClusterIndex = *aBeginClusterIndex + 1;
    }
}

void PathIterator::PrepareAttributeIndexRange(EndpointId aEndpointId, ClusterId aClusterId,
                                              const Optional<AttributeId> & aAttributeId, uint16_t * aBeginAttributeIndex,
                                              uint16_t * aEndAttributeIndex)
{
    if (!aAttributeId.HasValue())
    {
        *aBeginAttributeIndex = 0;
        *aEndAttributeIndex   = emberAfGetServerAttributeCount(aEndpointId, aClusterId);
    }
    else
    {
        *aBeginAttributeIndex = emberAfGetServerAttributeIndexByAttributeId(aEndpointId, aClusterId, aAttributeId.Value());
        *aEndAttributeIndex   = *aBeginAttributeIndex + 1;
    }
}

bool PathIterator::Proceed()
{
    for (; mpClusterInfo != nullptr; (mpClusterInfo = mpClusterInfo->mpNext, mEndpointIndex = UINT16_MAX))
    {
        // Special case: If this is a concrete path, we just return its value as-is.
        if (!mpClusterInfo->HasWildcard())
        {
            mOutputPath.mEndpointId  = mpClusterInfo->mEndpointId.Value();
            mOutputPath.mClusterId   = mpClusterInfo->mClusterId.Value();
            mOutputPath.mAttributeId = mpClusterInfo->mFieldId.Value();

            // Prepare for next iteration
            (mpClusterInfo = mpClusterInfo->mpNext, mEndpointIndex = UINT16_MAX);
            return true;
        }
        uint16_t beginEndpointIndex, endEndpointIndex;
        PrepareEndpointIndexRange(mpClusterInfo->mEndpointId, &beginEndpointIndex, &endEndpointIndex);

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
            PrepareClusterIndexRange(endpointId, mpClusterInfo->mClusterId, &beginClusterIndex, &endClusterIndex);

            if (mClusterIndex == UINT8_MAX)
            {
                // If we have not started iterating over the clusters yet.
                mClusterIndex   = beginClusterIndex;
                mAttributeIndex = UINT16_MAX;
            }

            for (; mClusterIndex < endClusterIndex; (mClusterIndex++, mAttributeIndex = UINT16_MAX))
            {
                ClusterId clusterId = emberAfGetNthClusterId(endpointId, mClusterIndex, true /* server */);
                uint16_t beginAttributeIndex, endAttributeIndex;
                PrepareAttributeIndexRange(endpointId, clusterId, mpClusterInfo->mFieldId, &beginAttributeIndex,
                                           &endAttributeIndex);
                if (mAttributeIndex == UINT16_MAX)
                {
                    // If we have not started iterating over the attributes yet.
                    mAttributeIndex = beginAttributeIndex;
                }

                if (mAttributeIndex < endAttributeIndex)
                {
                    mOutputPath.mAttributeId = emberAfGetServerAttributeIdByIndex(endpointId, clusterId, mAttributeIndex);
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
