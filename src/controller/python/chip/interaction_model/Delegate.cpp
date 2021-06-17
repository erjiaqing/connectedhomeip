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

#include <cinttypes>

#include <app/CommandSender.h>
#include <app/InteractionModelEngine.h>
#include <controller/python/chip/interaction_model/Delegate.h>
#include <support/logging/CHIPLogging.h>

using namespace chip::app;
using namespace chip::Controller;

namespace chip {

namespace Controller {

PythonInteractionModelDelegate gPythonInteractionModelDelegate;

CHIP_ERROR PythonInteractionModelDelegate::CommandResponseStatus(const CommandSender * apCommandSender,
                                                                 const Protocols::SecureChannel::GeneralStatusCode aGeneralCode,
                                                                 const uint32_t aProtocolId, const uint16_t aProtocolCode,
                                                                 chip::EndpointId aEndpointId, const chip::ClusterId aClusterId,
                                                                 chip::CommandId aCommandId, uint8_t aCommandIndex)
{
    CommandStatus status{ aProtocolId, aProtocolCode, aEndpointId, aClusterId, aCommandId, aCommandIndex };
    if (commandResponseStatusFunct != nullptr)
    {
        commandResponseStatusFunct(reinterpret_cast<uint64_t>(apCommandSender), &status, sizeof(status));
    }
    // For OpCred callbacks.
    DeviceControllerInteractionModelDelegate::CommandResponseStatus(apCommandSender, aGeneralCode, aProtocolId, aProtocolCode,
                                                                    aEndpointId, aClusterId, aCommandId, aCommandIndex);
    return CHIP_NO_ERROR;
}

CHIP_ERROR PythonInteractionModelDelegate::CommandResponseProtocolError(const CommandSender * apCommandSender,
                                                                        uint8_t aCommandIndex)
{
    if (commandResponseProtocolErrorFunct != nullptr)
    {
        commandResponseProtocolErrorFunct(reinterpret_cast<uint64_t>(apCommandSender), aCommandIndex);
    }
    DeviceControllerInteractionModelDelegate::CommandResponseProtocolError(apCommandSender, aCommandIndex);
    return CHIP_NO_ERROR;
}

CHIP_ERROR PythonInteractionModelDelegate::CommandResponseError(const CommandSender * apCommandSender, CHIP_ERROR aError)
{
    if (commandResponseErrorFunct != nullptr)
    {
        commandResponseErrorFunct(reinterpret_cast<uint64_t>(apCommandSender), aError);
    }
    if (aError != CHIP_NO_ERROR)
    {
        DeviceControllerInteractionModelDelegate::CommandResponseError(apCommandSender, aError);
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR PythonInteractionModelDelegate::CommandResponseProcessed(const app::CommandSender * apCommandSender)
{
    this->CommandResponseError(apCommandSender, CHIP_NO_ERROR);
    DeviceControllerInteractionModelDelegate::CommandResponseProcessed(apCommandSender);
    return CHIP_NO_ERROR;
}

void PythonInteractionModelDelegate::OnReportData(const app::ReadClient * apReadClient, const app::ClusterInfo & aPath,
                                                  TLV::TLVReader * apData, uint8_t status)
{
    ClusterAttributeInfo info;
    info.mAttributeId = aPath.mFieldId;
    info.mClusterId   = aPath.mClusterId;
    info.mEndpointId  = aPath.mEndpointId;
    if (reportDataFunct != nullptr && apReadClient->GetAppIdentifier() > UINT8_MAX)
    {
        TLV::TLVWriter writer;
        CHIP_ERROR err;
        uint8_t writerBuffer[1536];
        writer.Init(writerBuffer, sizeof(writerBuffer));
        err = writer.CopyElement(TLV::AnonymousTag, *apData);
        if (err != 0)
        {
            ChipLogError(Controller, "Failedf to build data: %" PRIu32, err);
        }
        reportDataFunct(apReadClient->GetAppIdentifier(), &info, sizeof(info), writerBuffer, writer.GetLengthWritten(), status);
    }
    if (apReadClient->GetAppIdentifier() <= UINT8_MAX)
    {
        DeviceControllerInteractionModelDelegate::OnReportData(apReadClient, aPath, apData, status);
    }
}

void PythonInteractionModelDelegate::OnReportEnd(const app::ReadClient * apReadClient, CHIP_ERROR aError)
{
    if (reportEndFunct != nullptr && apReadClient->GetAppIdentifier() > UINT8_MAX)
    {
        reportEndFunct(apReadClient->GetAppIdentifier(), aError);
    }
    if (apReadClient->GetAppIdentifier() <= UINT8_MAX)
    {
        DeviceControllerInteractionModelDelegate::OnReportEnd(apReadClient, aError);
    }
}

void pychip_InteractionModelDelegate_SetCommandResponseStatusCallback(
    PythonInteractionModelDelegate_OnCommandResponseStatusCodeReceivedFunct f)
{
    gPythonInteractionModelDelegate.SetOnCommandResponseStatusCodeReceivedCallback(f);
}

void pychip_InteractionModelDelegate_SetCommandResponseProtocolErrorCallback(
    PythonInteractionModelDelegate_OnCommandResponseProtocolErrorFunct f)
{
    gPythonInteractionModelDelegate.SetOnCommandResponseProtocolErrorCallback(f);
}

void pychip_InteractionModelDelegate_SetCommandResponseErrorCallback(PythonInteractionModelDelegate_OnCommandResponseFunct f)
{
    gPythonInteractionModelDelegate.SetOnCommandResponseCallback(f);
}

void pychip_InteractionModelDelegate_SetOnReportDataCallback(PythonInteractionModelDelegate_OnReportDataFunct f)
{
    gPythonInteractionModelDelegate.SetOnReportDataCallback(f);
}

void pychip_InteractionModelDelegate_SetOnReportEndCallback(PythonInteractionModelDelegate_OnReportEndFunct f)
{
    gPythonInteractionModelDelegate.SetOnReportEndCallback(f);
}

PythonInteractionModelDelegate & PythonInteractionModelDelegate::Instance()
{
    return gPythonInteractionModelDelegate;
}

} // namespace Controller
} // namespace chip

extern "C" {

CHIP_ERROR pychip_InteractionModel_GetCommandSenderHandle(uint64_t * commandSender)
{
    chip::app::CommandSender * commandSenderObj = nullptr;
    VerifyOrReturnError(commandSender != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    ReturnErrorOnFailure(chip::app::InteractionModelEngine::GetInstance()->NewCommandSender(&commandSenderObj));
    *commandSender = reinterpret_cast<uint64_t>(commandSenderObj);
    return CHIP_NO_ERROR;
}
}
