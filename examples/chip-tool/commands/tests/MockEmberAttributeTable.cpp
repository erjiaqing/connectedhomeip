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

#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPTLVDebug.hpp>
#include <lib/core/DataModelTypes.h>
#include <lib/support/CodeUtils.h>

using namespace chip;

// Mock Functions
typedef uint8_t EmberAfClusterMask;
#define CLUSTER_MASK_SERVER (0x40)

namespace {

EndpointId endpoints[]    = { 200, 201, 202 };
uint16_t clusterIndex[]   = { 0, 2, 5 };
uint8_t clusterCount[]    = { 2, 3, 4 };
ClusterId clusters[]      = { 1, 2, 1, 2, 3, 1, 2, 3, 4 };
uint16_t attributeIndex[] = { 0, 2, 5, 7, 11, 16, 19, 25, 27 };
uint16_t attributeCount[] = { 2, 3, 2, 4, 5, 3, 6, 2, 1 };
AttributeId attributes[]  = {
    // clang-format off
    100, 2,
    100, 4, 5,
    100, 2,
    100, 4, 5, 6,
    100, 8, 9, 10, 11,
    100, 2, 3,
    100, 4, 5, 6, 7, 8,
    100, 8,
    100,
    // clang-format on
};

} // namespace

uint16_t emberAfEndpointCount(void)
{
    return ArraySize(endpoints);
}

uint16_t emberAfIndexFromEndpoint(chip::EndpointId endpoint)
{
    for (uint16_t i = 0; i < ArraySize(endpoints); i++)
    {
        if (endpoints[i] == endpoint)
        {
            return i;
        }
    }
    return UINT16_MAX;
}

uint8_t emberAfClusterCount(chip::EndpointId endpoint, bool server)
{
    for (uint16_t i = 0; i < ArraySize(endpoints); i++)
    {
        if (endpoints[i] == endpoint)
        {
            return clusterCount[i];
        }
    }
    return 0;
}

uint16_t emberAfGetServerAttributeCount(chip::EndpointId endpoint, chip::ClusterId cluster)
{
    uint16_t endpointIndex          = emberAfIndexFromEndpoint(endpoint);
    uint16_t clusterCountOnEndpoint = emberAfClusterCount(endpoint, true);
    for (uint16_t i = 0; i < clusterCountOnEndpoint; i++)
    {
        if (clusters[i + clusterIndex[endpointIndex]] == cluster)
        {
            return attributeCount[i + clusterIndex[endpointIndex]];
        }
    }
    return 0xFFFF;
}

uint16_t emberAfGetServerAttributeIndexByAttributeId(chip::EndpointId endpoint, chip::ClusterId cluster,
                                                     chip::AttributeId attributeId)
{
    uint16_t endpointIndex          = emberAfIndexFromEndpoint(endpoint);
    uint16_t clusterCountOnEndpoint = emberAfClusterCount(endpoint, true);
    for (uint16_t i = 0; i < clusterCountOnEndpoint; i++)
    {
        if (clusters[i + clusterIndex[endpointIndex]] == cluster)
        {
            uint16_t clusterAttributeOffset = attributeIndex[i + clusterIndex[endpointIndex]];
            for (uint16_t j = 0; j < emberAfGetServerAttributeCount(endpoint, cluster); j++)
            {
                if (attributes[clusterAttributeOffset + j] == attributeId)
                {
                    return j;
                }
            }
            return 0xFFFF;
        }
    }
    return 0xFFFF;
}

chip::EndpointId emberAfEndpointFromIndex(uint16_t index)
{
    return index < ArraySize(endpoints) ? endpoints[index] : chip::kWildcardEndpointId;
}

chip::ClusterId emberAfGetNthClusterId(chip::EndpointId endpoint, uint8_t n, bool server)
{
    if (n >= emberAfClusterCount(endpoint, server))
    {
        return MEI::kWildcard;
    }
    return clusters[clusterIndex[emberAfIndexFromEndpoint(endpoint)] + n];
}

chip::AttributeId emberAfGetServerAttributeIdByIndex(chip::EndpointId endpoint, chip::ClusterId cluster, uint16_t index)
{
    uint16_t endpointIndex          = emberAfIndexFromEndpoint(endpoint);
    uint16_t clusterCountOnEndpoint = emberAfClusterCount(endpoint, true);
    for (uint16_t i = 0; i < clusterCountOnEndpoint; i++)
    {
        if (clusters[i + clusterIndex[endpointIndex]] == cluster)
        {
            uint16_t clusterAttributeOffset = attributeIndex[i + clusterIndex[endpointIndex]];
            if (index >= emberAfGetServerAttributeCount(endpoint, cluster))
            {
                return MEI::kWildcard;
            }
            return attributes[clusterAttributeOffset + index];
        }
    }
    return 0xFFFF;
}

uint8_t emberAfClusterIndex(chip::EndpointId endpoint, chip::ClusterId cluster, EmberAfClusterMask mask)
{
    uint16_t endpointIndex          = emberAfIndexFromEndpoint(endpoint);
    uint16_t clusterCountOnEndpoint = emberAfClusterCount(endpoint, true);
    for (uint8_t i = 0; i < clusterCountOnEndpoint; i++)
    {
        if (clusters[i + clusterIndex[endpointIndex]] == cluster)
        {
            return i;
        }
    }
    return 0xFF;
}
