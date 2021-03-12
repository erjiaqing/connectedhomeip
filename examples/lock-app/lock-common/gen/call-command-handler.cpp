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

// THIS FILE IS GENERATED BY ZAP

#include <stdint.h>

#include "af-structs.h"
#include "call-command-handler.h"
#include "callback.h"
#include "cluster-id.h"
#include "command-id.h"
#include "util.h"

using namespace chip;

EmberAfStatus emberAfBasicClusterServerCommandParse(EmberAfClusterCommand * cmd);
EmberAfStatus emberAfNetworkCommissioningClusterServerCommandParse(EmberAfClusterCommand * cmd);
EmberAfStatus emberAfOnOffClusterServerCommandParse(EmberAfClusterCommand * cmd);

static EmberAfStatus status(bool wasHandled, bool clusterExists, bool mfgSpecific)
{
    if (wasHandled)
    {
        return EMBER_ZCL_STATUS_SUCCESS;
    }
    else if (mfgSpecific)
    {
        return EMBER_ZCL_STATUS_UNSUP_MANUF_CLUSTER_COMMAND;
    }
    else if (clusterExists)
    {
        return EMBER_ZCL_STATUS_UNSUP_COMMAND;
    }
    else
    {
        return EMBER_ZCL_STATUS_UNSUPPORTED_CLUSTER;
    }
}

// Main command parsing controller.
EmberAfStatus emberAfClusterSpecificCommandParse(EmberAfClusterCommand * cmd)
{
    EmberAfStatus result = status(false, false, cmd->mfgSpecific);
    if (cmd->direction == (uint8_t) ZCL_DIRECTION_SERVER_TO_CLIENT &&
        emberAfContainsClientWithMfgCode(cmd->apsFrame->destinationEndpoint, cmd->apsFrame->clusterId, cmd->mfgCode))
    {
        switch (cmd->apsFrame->clusterId)
        {
        default:
            // Unrecognized cluster ID, error status will apply.
            break;
        }
    }
    else if (cmd->direction == (uint8_t) ZCL_DIRECTION_CLIENT_TO_SERVER &&
             emberAfContainsServerWithMfgCode(cmd->apsFrame->destinationEndpoint, cmd->apsFrame->clusterId, cmd->mfgCode))
    {
        switch (cmd->apsFrame->clusterId)
        {
        case ZCL_BASIC_CLUSTER_ID:
            // No commands are enabled for cluster Basic
            result = status(false, true, cmd->mfgSpecific);
            break;
        case ZCL_NETWORK_COMMISSIONING_CLUSTER_ID:
            result = emberAfNetworkCommissioningClusterServerCommandParse(cmd);
            break;
        case ZCL_ON_OFF_CLUSTER_ID:
            result = emberAfOnOffClusterServerCommandParse(cmd);
            break;
        default:
            // Unrecognized cluster ID, error status will apply.
            break;
        }
    }
    return result;
}

// Cluster specific command parsing

EmberAfStatus emberAfNetworkCommissioningClusterServerCommandParse(EmberAfClusterCommand * cmd)
{
    bool wasHandled = false;

    if (!cmd->mfgSpecific)
    {
        switch (cmd->commandId)
        {
        case ZCL_ADD_THREAD_NETWORK_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * operationalDataset;
            uint64_t breadcrumb;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            operationalDataset = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset      = static_cast<uint16_t>(payloadOffset + emberAfStringLength(operationalDataset) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterAddThreadNetworkCallback(operationalDataset, breadcrumb, timeoutMs);
            break;
        }
        case ZCL_ADD_WI_FI_NETWORK_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * ssid;
            uint8_t * credentials;
            uint64_t breadcrumb;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            ssid          = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(ssid) + 1u);
            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            credentials   = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(credentials) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterAddWiFiNetworkCallback(ssid, credentials, breadcrumb, timeoutMs);
            break;
        }
        case ZCL_DISABLE_NETWORK_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * networkID;
            uint64_t breadcrumb;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            networkID     = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(networkID) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterDisableNetworkCallback(networkID, breadcrumb, timeoutMs);
            break;
        }
        case ZCL_ENABLE_NETWORK_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * networkID;
            uint64_t breadcrumb;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            networkID     = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(networkID) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterEnableNetworkCallback(networkID, breadcrumb, timeoutMs);
            break;
        }
        case ZCL_GET_LAST_NETWORK_COMMISSIONING_RESULT_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterGetLastNetworkCommissioningResultCallback(timeoutMs);
            break;
        }
        case ZCL_REMOVE_NETWORK_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * NetworkID;
            uint64_t Breadcrumb;
            uint32_t TimeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            NetworkID     = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(NetworkID) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            Breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            TimeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterRemoveNetworkCallback(NetworkID, Breadcrumb, TimeoutMs);
            break;
        }
        case ZCL_SCAN_NETWORKS_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * ssid;
            uint64_t breadcrumb;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            ssid          = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(ssid) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterScanNetworksCallback(ssid, breadcrumb, timeoutMs);
            break;
        }
        case ZCL_UPDATE_THREAD_NETWORK_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * operationalDataset;
            uint64_t breadcrumb;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            operationalDataset = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset      = static_cast<uint16_t>(payloadOffset + emberAfStringLength(operationalDataset) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterUpdateThreadNetworkCallback(operationalDataset, breadcrumb, timeoutMs);
            break;
        }
        case ZCL_UPDATE_WI_FI_NETWORK_COMMAND_ID: {
            uint16_t payloadOffset = cmd->payloadStartIndex;
            uint8_t * ssid;
            uint8_t * credentials;
            uint64_t breadcrumb;
            uint32_t timeoutMs;

            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            ssid          = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(ssid) + 1u);
            if (cmd->bufLen < payloadOffset + 1u)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            credentials   = emberAfGetString(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + emberAfStringLength(credentials) + 1u);
            if (cmd->bufLen < payloadOffset + 8)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            breadcrumb    = emberAfGetInt64u(cmd->buffer, payloadOffset, cmd->bufLen);
            payloadOffset = static_cast<uint16_t>(payloadOffset + 8);
            if (cmd->bufLen < payloadOffset + 4)
            {
                return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
            }
            timeoutMs = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);

            wasHandled = emberAfNetworkCommissioningClusterUpdateWiFiNetworkCallback(ssid, credentials, breadcrumb, timeoutMs);
            break;
        }
        default: {
            // Unrecognized command ID, error status will apply.
            break;
        }
        }
    }
    return status(wasHandled, true, cmd->mfgSpecific);
}
EmberAfStatus emberAfOnOffClusterServerCommandParse(EmberAfClusterCommand * cmd)
{
    bool wasHandled = false;

    if (!cmd->mfgSpecific)
    {
        switch (cmd->commandId)
        {
        case ZCL_OFF_COMMAND_ID: {
            wasHandled = emberAfOnOffClusterOffCallback();
            break;
        }
        case ZCL_ON_COMMAND_ID: {
            wasHandled = emberAfOnOffClusterOnCallback();
            break;
        }
        case ZCL_TOGGLE_COMMAND_ID: {
            wasHandled = emberAfOnOffClusterToggleCallback();
            break;
        }
        default: {
            // Unrecognized command ID, error status will apply.
            break;
        }
        }
    }
    return status(wasHandled, true, cmd->mfgSpecific);
}
