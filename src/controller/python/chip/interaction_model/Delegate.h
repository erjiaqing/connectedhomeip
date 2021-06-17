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

#pragma once

#include <app/InteractionModelDelegate.h>
#include <controller/CHIPDeviceController.h>

namespace chip {
namespace Controller {

// The command status will be used for python script.
// use packed attribute so we can unpack it from python and no need to worry about padding.
struct __attribute__((packed)) CommandStatus
{
    uint32_t ProtocolId;
    uint16_t ProtocolCode;
    chip::EndpointId EndpointId;
    chip::ClusterId ClusterId;
    chip::CommandId CommandId;
    uint8_t CommandIndex;
};

struct __attribute__((packed)) ClusterAttributeInfo
{
    chip::ClusterId mClusterId;
    chip::AttributeId mAttributeId;
    chip::EndpointId mEndpointId;
};

extern "C" {
typedef void (*PythonInteractionModelDelegate_OnCommandResponseStatusCodeReceivedFunct)(uint64_t commandSenderPtr,
                                                                                        void * commandStatusBuf,
                                                                                        uint32_t commandStatusBufLen);
typedef void (*PythonInteractionModelDelegate_OnCommandResponseProtocolErrorFunct)(uint64_t commandSenderPtr, uint8_t commandIndex);
typedef void (*PythonInteractionModelDelegate_OnCommandResponseFunct)(uint64_t commandSenderPtr, uint32_t error);

typedef void (*PythonInteractionModelDelegate_OnReportDataFunct)(uint64_t aApplicationIdentifier,
                                                                 const void * apClusterAttributeInfoBuf,
                                                                 size_t aClusterAttributeInfoBufLen, const uint8_t * apDataBuf,
                                                                 size_t aDataBufLength, uint8_t status);
typedef void (*PythonInteractionModelDelegate_OnReportEndFunct)(uint64_t aApplicationIdentifier, CHIP_ERROR aError);

void pychip_InteractionModelDelegate_SetCommandResponseStatusCallback(
    PythonInteractionModelDelegate_OnCommandResponseStatusCodeReceivedFunct f);
void pychip_InteractionModelDelegate_SetCommandResponseProtocolErrorCallback(
    PythonInteractionModelDelegate_OnCommandResponseProtocolErrorFunct f);
void pychip_InteractionModelDelegate_SetCommandResponseErrorCallback(PythonInteractionModelDelegate_OnCommandResponseFunct f);
void pychip_InteractionModelDelegate_SetOnReportDataCallback(PythonInteractionModelDelegate_OnReportDataFunct f);
void pychip_InteractionModelDelegate_SetOnReportEndCallback(PythonInteractionModelDelegate_OnReportEndFunct f);
}

class PythonInteractionModelDelegate : public chip::Controller::DeviceControllerInteractionModelDelegate
{
public:
    void OnReportData(const app::ReadClient * apReadClient, const app::ClusterInfo & aPath, TLV::TLVReader * apData,
                      uint8_t status) override;
    void OnReportEnd(const app::ReadClient * apReadClient, CHIP_ERROR aError) override;

    CHIP_ERROR CommandResponseStatus(const app::CommandSender * apCommandSender,
                                     const Protocols::SecureChannel::GeneralStatusCode aGeneralCode, const uint32_t aProtocolId,
                                     const uint16_t aProtocolCode, chip::EndpointId aEndpointId, const chip::ClusterId aClusterId,
                                     chip::CommandId aCommandId, uint8_t aCommandIndex) override;

    CHIP_ERROR CommandResponseProtocolError(const app::CommandSender * apCommandSender, uint8_t aCommandIndex) override;

    CHIP_ERROR CommandResponseError(const app::CommandSender * apCommandSender, CHIP_ERROR aError) override;

    CHIP_ERROR CommandResponseProcessed(const app::CommandSender * apCommandSender) override;

    static PythonInteractionModelDelegate & Instance();

    void SetOnCommandResponseStatusCodeReceivedCallback(PythonInteractionModelDelegate_OnCommandResponseStatusCodeReceivedFunct f)
    {
        commandResponseStatusFunct = f;
    }

    void SetOnCommandResponseProtocolErrorCallback(PythonInteractionModelDelegate_OnCommandResponseProtocolErrorFunct f)
    {
        commandResponseProtocolErrorFunct = f;
    }

    void SetOnCommandResponseCallback(PythonInteractionModelDelegate_OnCommandResponseFunct f) { commandResponseErrorFunct = f; }

    void SetOnReportDataCallback(PythonInteractionModelDelegate_OnReportDataFunct f) { reportDataFunct = f; }
    void SetOnReportEndCallback(PythonInteractionModelDelegate_OnReportEndFunct f) { reportEndFunct = f; }

private:
    PythonInteractionModelDelegate_OnCommandResponseStatusCodeReceivedFunct commandResponseStatusFunct   = nullptr;
    PythonInteractionModelDelegate_OnCommandResponseProtocolErrorFunct commandResponseProtocolErrorFunct = nullptr;
    PythonInteractionModelDelegate_OnCommandResponseFunct commandResponseErrorFunct                      = nullptr;

    PythonInteractionModelDelegate_OnReportDataFunct reportDataFunct = nullptr;
    PythonInteractionModelDelegate_OnReportEndFunct reportEndFunct   = nullptr;
};

} // namespace Controller
} // namespace chip
