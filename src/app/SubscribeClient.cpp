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
 *      This file defines the initiator side of a CHIP Read Interaction.
 *
 */

#include <app/AppBuildConfig.h>
#include <app/InteractionModelEngine.h>
#include <app/SubscribeClient.h>
#include <app/MessageDef/SubscribeRequest.h>
#include <app/MessageDef/SubscribeResponse.h>
#include <protocols/secure_channel/StatusReport.h>
#include <lib/support/TypeTraits.h>

namespace chip {
namespace app {
CHIP_ERROR SubscribeClient::Init(Messaging::ExchangeManager * apExchangeMgr, InteractionModelDelegate * apDelegate,
                                 uint64_t aAppIdentifier)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    err = ReadClient::Init(apExchangeMgr, apDelegate, aAppIdentifier);
    mRetryCounter = 0;
    mFinalSyncIntervalSeconds = 0;
    mSubscriptionId = 0;
    EnableResubscribe(true);
    return err;
}

void SubscribeClient::ShutdownInternal()
{
    mRetryCounter = 0;
    mFinalSyncIntervalSeconds = 0;
    mSubscriptionId = 0;
    EnableResubscribe(false);
    CancelLivenessCheckTimer();
    ReadClient::ShutdownInternal();
}

CHIP_ERROR SubscribeClient::SendSubscribeRequest()
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    System::PacketBufferHandle msgBuf;
    SubscribePrepareParams subscribePrepareParams;
    VerifyOrExit(ClientState::Initialized == mState, err = CHIP_ERROR_INCORRECT_STATE);
    VerifyOrExit(mpDelegate != nullptr, err = CHIP_ERROR_INCORRECT_STATE);

    AbortExistingExchangeContext();

    mpDelegate->SubscriptionPrepareNeeded(subscribePrepareParams);

    {
        System::PacketBufferTLVWriter writer;
        SubscribeRequest::Builder request;

        msgBuf = System::PacketBufferHandle::New(kMaxSecureSduLengthBytes);
        VerifyOrExit(!msgBuf.IsNull(), err = CHIP_ERROR_NO_MEMORY);

        writer.Init(std::move(msgBuf));

        err = request.Init(&writer);
        SuccessOrExit(err);

        if (subscribePrepareParams.mEventPathParamsListSize != 0 && subscribePrepareParams.mpEventPathParamsList != nullptr)
        {
            EventPathList::Builder & eventPathListBuilder = request.CreateEventPathListBuilder();
            SuccessOrExit(err = eventPathListBuilder.GetError());
            err = GenerateEventPathList(eventPathListBuilder, subscribePrepareParams.mpEventPathParamsList, subscribePrepareParams.mEventPathParamsListSize);
            SuccessOrExit(err);

            if (subscribePrepareParams.mEventNumber != 0)
            {
                // EventNumber is optional
                request.EventNumber(subscribePrepareParams.mEventNumber);
            }
        }

        if (subscribePrepareParams.mAttributePathParamsListSize != 0 && subscribePrepareParams.mpAttributePathParamsList != nullptr)
        {
            AttributePathList::Builder & attributePathListBuilder = request.CreateAttributePathListBuilder();
            SuccessOrExit(err = attributePathListBuilder.GetError());
            err = GenerateAttributePathList(attributePathListBuilder, subscribePrepareParams.mpAttributePathParamsList, subscribePrepareParams.mAttributePathParamsListSize);
            SuccessOrExit(err);
        }

        request.MinIntervalSeconds(subscribePrepareParams.mMinIntervalSeconds).MaxIntervalSeconds(subscribePrepareParams.mMaxIntervalSeconds).EndOfSubscribeRequest();
        SuccessOrExit(err = request.GetError());

        err = writer.Finalize(&msgBuf);
        SuccessOrExit(err);
    }

    if (subscribePrepareParams.mpSecureSession != nullptr)
    {
        mpExchangeCtx = mpExchangeMgr->NewContext(*(subscribePrepareParams.mpSecureSession), this);
    }
    else
    {
        mpExchangeCtx = mpExchangeMgr->NewContext({ subscribePrepareParams.mNodeId, 0, subscribePrepareParams.mFabricIndex }, this);
    }
    VerifyOrExit(mpExchangeCtx != nullptr, err = CHIP_ERROR_NO_MEMORY);
    mpExchangeCtx->SetResponseTimeout(kImMessageTimeoutMsec);

    err = mpExchangeCtx->SendMessage(Protocols::InteractionModel::MsgType::SubscribeRequest, std::move(msgBuf),
                                     Messaging::SendFlags(Messaging::SendMessageFlags::kExpectResponse));
    SuccessOrExit(err);
    MoveToState(ClientState::Subscribing);

exit:
    ChipLogFunctError(err);

    if (err != CHIP_NO_ERROR)
    {
        AbortExistingExchangeContext();
    }

    return err;
}

CHIP_ERROR SubscribeClient::OnMessageReceived(Messaging::ExchangeContext * apExchangeContext, const PacketHeader & aPacketHeader,
                                         const PayloadHeader & aPayloadHeader, System::PacketBufferHandle && aPayload)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    VerifyOrExit(mpDelegate != nullptr, err = CHIP_ERROR_INCORRECT_STATE);
    if (aPayloadHeader.HasMessageType(Protocols::InteractionModel::MsgType::SubscribeResponse))
    {
        VerifyOrExit(apExchangeContext == mpExchangeCtx, err = CHIP_ERROR_INCORRECT_STATE);
        err = ProcessSubscribeResponse(std::move(aPayload));
        if (err != CHIP_NO_ERROR)
        {
            mpDelegate->SubscribeError(this, err);
        }
        else
        {
            mpDelegate->SubscribeResponseProcessed(this);
        }
    }
    else if (aPayloadHeader.HasMessageType(Protocols::InteractionModel::MsgType::ReportData))
    {
        err = ProcessReportData(std::move(aPayload));
        if (err != CHIP_NO_ERROR)
        {
            mpDelegate->ReportError(this, err);
        }
        else
        {
            mpDelegate->ReportProcessed(this);
        }
    }
    else
    {
        err = CHIP_ERROR_INVALID_MESSAGE_TYPE;
    }

exit:
    ChipLogFunctError(err);
    if (err != CHIP_NO_ERROR)
    {
        ShutdownInternal();
    }
    return err;
}

CHIP_ERROR SubscribeClient::ProcessSubscribeResponse(System::PacketBufferHandle && aPayload)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    SubscribeResponse::Parser subscribeResponse;
    System::PacketBufferTLVReader reader;
    reader.Init(std::move(aPayload));
    reader.Next();

    err = subscribeResponse.Init(reader);
    SuccessOrExit(err);

#if CHIP_CONFIG_IM_ENABLE_SCHEMA_CHECK
    err = subscribeResponse.CheckSchemaValidity();
    SuccessOrExit(err);
#endif

    err = subscribeResponse.GetSubscriptionId(&mSubscriptionId);
    if (CHIP_END_OF_TLV == err)
    {
        err = CHIP_NO_ERROR;
    }
    SuccessOrExit(err);

    err = subscribeResponse.GetFinalSyncIntervalSeconds(&mFinalSyncIntervalSeconds);
    if (CHIP_END_OF_TLV == err)
    {
        err = CHIP_NO_ERROR;
    }

exit:
    SendStatusReport(err);
    ChipLogFunctError(err);
    return err;
}

void SubscribeClient::OnResponseTimeout(Messaging::ExchangeContext * apExchangeContext)
{
    ChipLogProgress(DataManagement, "Time out! failed to receive report data from Exchange: %d",
                    apExchangeContext->GetExchangeId());
    if (nullptr != mpDelegate)
    {
        if (ClientState::Subscribing == mState)
        {
            mpDelegate->SubscribeError(this, CHIP_ERROR_TIMEOUT);
        }
        else if (ClientState::SubscriptionIdle == mState)
        {
            mpDelegate->ReportError(this, CHIP_ERROR_TIMEOUT);
        }
    }

    RefreshLivenessCheckTimer();
}

CHIP_ERROR SubscribeClient::SendStatusReport(CHIP_ERROR aError)
{
    Protocols::SecureChannel::GeneralStatusCode generalCode = Protocols::SecureChannel::GeneralStatusCode::kSuccess;
    uint32_t protocolId           = Protocols::InteractionModel::Id.ToFullyQualifiedSpecForm();
    uint16_t protocolCode         = to_underlying(Protocols::InteractionModel::ProtocolCode::Success);
    VerifyOrReturnLogError(mpExchangeCtx != nullptr, CHIP_ERROR_NO_MEMORY);
    // Need to add chunk support for multiple report
    if (aError != CHIP_NO_ERROR)
    {
        generalCode = Protocols::SecureChannel::GeneralStatusCode::kFailure;
        protocolCode = to_underlying(Protocols::InteractionModel::ProtocolCode::InvalidSubscription);
    }

    ChipLogProgress(DataManagement, "SendStatusReport ");
    Protocols::SecureChannel::StatusReport report(generalCode, protocolId, protocolCode);

    Encoding::LittleEndian::PacketBufferWriter buf(System::PacketBufferHandle::New(kMaxSecureSduLengthBytes));
    report.WriteToBuffer(buf);
    System::PacketBufferHandle msgBuf = buf.Finalize();
    VerifyOrReturnLogError(!msgBuf.IsNull(), CHIP_ERROR_NO_MEMORY);

    ReturnLogErrorOnFailure(mpExchangeCtx->SendMessage(Protocols::SecureChannel::MsgType::StatusReport, std::move(msgBuf), Messaging::SendFlags(Messaging::SendMessageFlags::kExpectResponse)));
    MoveToState(ClientState::SubscriptionIdle);
    return CHIP_NO_ERROR;
}

CHIP_ERROR SubscribeClient::ProcessReportData(System::PacketBufferHandle && aPayload)
{
    ReturnLogErrorOnFailure(ReadClient::ProcessReportData(std::move(aPayload)));
    ReturnLogErrorOnFailure(RefreshLivenessCheckTimer());
    return CHIP_NO_ERROR;
}

CHIP_ERROR SubscribeClient::RefreshLivenessCheckTimer()
{
    CHIP_ERROR err                   = CHIP_NO_ERROR;
    CancelLivenessCheckTimer();
    ChipLogProgress(DataManagement, "SubscribeClient::RefreshLivenessCheckTime timer %d", mFinalSyncIntervalSeconds);
    err = InteractionModelEngine::GetInstance()->GetExchangeManager()->GetSessionMgr()->SystemLayer()->StartTimer(
                mFinalSyncIntervalSeconds, OnLivenessTimeoutCallback, this);

    if (err != CHIP_NO_ERROR)
    {
        ShutdownInternal();
    }
    return err;
}

void SubscribeClient::CancelLivenessCheckTimer()
{
   InteractionModelEngine::GetInstance()->GetExchangeManager()->GetSessionMgr()->SystemLayer()->CancelTimer(OnLivenessTimeoutCallback, this);
}

void SubscribeClient::OnLivenessTimeoutCallback(System::Layer * apSystemLayer, void * apAppState)
{
    CHIP_ERROR err                   = CHIP_NO_ERROR;
    uint16_t retryTimeSec = 0;
    SubscribeClient * const client = reinterpret_cast<SubscribeClient *>(apAppState);
    if ((nullptr != client->mpDelegate) && (ClientState::Uninitialized != client->mState))
    {
        client->mpDelegate->ApplyResubscribePolicy(client->mRetryCounter, retryTimeSec);
        err = InteractionModelEngine::GetInstance()->GetExchangeManager()->GetSessionMgr()->SystemLayer()->StartTimer(
                retryTimeSec, OnReSubscribeTimerCallback, apAppState);
    }
}

void SubscribeClient::OnReSubscribeTimerCallback(System::Layer * apSystemLayer, void * apAppState)
{
    SubscribeClient * const client = reinterpret_cast<SubscribeClient *>(apAppState);
    if (client != nullptr && client->mEnableResubscribe && (client->mState != ClientState::Uninitialized))
    {
        client->mRetryCounter ++;
        client->SendSubscribeRequest();
    }
}

/**
 * @brief Kick the resubscribe mechanism. This will initiate an immediate retry if resubscribe is enabled
 */
void SubscribeClient::ResetResubscribe()
{
    mRetryCounter = 0;
    mSubscriptionId = 0;
    mFinalSyncIntervalSeconds = 0;
    if (mState != ClientState::Uninitialized)
    {
        CancelLivenessCheckTimer();
        CancelResubscribe();
        if (mEnableResubscribe)
        {
            InteractionModelEngine::GetInstance()->GetExchangeManager()->GetSessionMgr()->SystemLayer()->StartTimer(
                    0, OnReSubscribeTimerCallback, this);
        }
    }
}

void SubscribeClient::CancelResubscribe()
{
    InteractionModelEngine::GetInstance()->GetExchangeManager()->GetSessionMgr()->SystemLayer()->CancelTimer(OnReSubscribeTimerCallback, this);
}

/**
 * @brief Enable/Disable the resubscribe mechanism. if disable, this will abort if a resubscribe
 * was pending.
 */
void SubscribeClient::EnableResubscribe(bool aEnable)
{
    mEnableResubscribe = aEnable;
    if (!mEnableResubscribe)
    {
        CancelLivenessCheckTimer();
        CancelResubscribe();
    }
}
} // namespace app
} // namespace chip
