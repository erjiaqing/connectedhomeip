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
 *      This file defines the initiator side of a CHIP Write Interaction.
 *
 */

#include <app/AppBuildConfig.h>
#include <app/InteractionModelEngine.h>
#include <app/WriteClient.h>

namespace chip {
namespace app {

CHIP_ERROR WriteClient::AllocateBuffer()
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    AttributeDataList::Builder attributeDataListBuilder;

    if (!mBufferAllocated)
    {
        mMessageWriter.Reset();

        System::PacketBufferHandle commandPacket = System::PacketBufferHandle::New(chip::app::kMaxSecureSduLengthBytes);
        VerifyOrExit(!commandPacket.IsNull(), err = CHIP_ERROR_NO_MEMORY);

        mMessageWriter.Init(std::move(commandPacket));
        err = mWriteRequestBuilder.Init(&mMessageWriter);
        SuccessOrExit(err);

        attributeDataListBuilder = mWriteRequestBuilder.CreateAttributeDataListBuilder();
        SuccessOrExit(attributeDataListBuilder.GetError());

        mAttributeStatusIndex = 0;

        mBufferAllocated = true;
    }

exit:
    return err;
}

void WriteClient::Abort()
{
    //
    // If the exchange context hasn't already been gracefully closed
    // (signaled by setting it to null), then we need to forcibly
    // tear it down.
    //
    if (mpExchangeCtx != nullptr)
    {
        mpExchangeCtx->Abort();
        mpExchangeCtx = nullptr;
    }
}

void WriteClient::Close()
{
    //
    // Shortly after this call to close and when handling an inbound message, it's entirely possible
    // for this object (courtesy of its derived class) to be destroyed
    // *before* the call unwinds all the way back to ExchangeContext::HandleMessage.
    //
    // As part of tearing down the exchange, there is logic there to invoke the delegate to notify
    // it of impending closure - which is this object, which just got destroyed!
    //
    // So prevent a use-after-free, set delegate to null.
    //
    // For more details, see #10344.
    //
    if (mpExchangeCtx != nullptr)
    {
        mpExchangeCtx->SetDelegate(nullptr);
    }

    mpExchangeCtx = nullptr;

    if (mpCallback)
    {
        mpCallback->OnDone(this);
    }
}

CHIP_ERROR WriteClient::ProcessWriteResponseMessage(System::PacketBufferHandle && payload)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    System::PacketBufferTLVReader reader;
    TLV::TLVReader attributeStatusListReader;
    WriteResponse::Parser writeResponse;
    AttributeStatusList::Parser attributeStatusListParser;

    reader.Init(std::move(payload));
    err = reader.Next();
    SuccessOrExit(err);

    err = writeResponse.Init(reader);
    SuccessOrExit(err);

#if CHIP_CONFIG_IM_ENABLE_SCHEMA_CHECK
    err = writeResponse.CheckSchemaValidity();
    SuccessOrExit(err);
#endif
    err = writeResponse.GetAttributeStatusList(&attributeStatusListParser);
    SuccessOrExit(err);

    attributeStatusListParser.GetReader(&attributeStatusListReader);

    while (CHIP_NO_ERROR == (err = attributeStatusListReader.Next()))
    {
        VerifyOrExit(TLV::AnonymousTag == attributeStatusListReader.GetTag(), err = CHIP_ERROR_INVALID_TLV_TAG);
        VerifyOrExit(TLV::kTLVType_Structure == attributeStatusListReader.GetType(), err = CHIP_ERROR_WRONG_TLV_TYPE);

        AttributeStatusElement::Parser element;

        err = element.Init(attributeStatusListReader);
        SuccessOrExit(err);

        err = ProcessAttributeStatusElement(element);
        SuccessOrExit(err);
    }

    // if we have exhausted this container
    if (CHIP_END_OF_TLV == err)
    {
        err = CHIP_NO_ERROR;
    }

exit:
    return err;
}

CHIP_ERROR WriteClient::PrepareAttribute(const AttributePathParams & attributePathParams)
{
    ReturnErrorOnFailure(AllocateBuffer());

    AttributeDataElement::Builder attributeDataElement =
        mWriteRequestBuilder.GetAttributeDataListBuilder().CreateAttributeDataElementBuilder();
    ReturnErrorOnFailure(attributeDataElement.GetError());

    return ConstructAttributePath(attributePathParams, attributeDataElement);
}

CHIP_ERROR WriteClient::FinishAttribute()
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    AttributeDataElement::Builder attributeDataElement =
        mWriteRequestBuilder.GetAttributeDataListBuilder().GetAttributeDataElementBuilder();

    // TODO: Add attribute version support
    attributeDataElement.DataVersion(0);
    attributeDataElement.EndOfAttributeDataElement();
    SuccessOrExit(err = attributeDataElement.GetError());
    MoveToState(State::AddAttribute);

exit:
    return err;
}

TLV::TLVWriter * WriteClient::GetAttributeDataElementTLVWriter()
{
    return mWriteRequestBuilder.GetAttributeDataListBuilder().GetAttributeDataElementBuilder().GetWriter();
}

CHIP_ERROR WriteClient::ConstructAttributePath(const AttributePathParams & aAttributePathParams,
                                               AttributeDataElement::Builder aAttributeDataElement)
{
    AttributePath::Builder attributePath = aAttributeDataElement.CreateAttributePathBuilder();
    if (aAttributePathParams.mFlags.Has(AttributePathParams::Flags::kFieldIdValid))
    {
        attributePath.FieldId(aAttributePathParams.mFieldId);
    }

    if (aAttributePathParams.mFlags.Has(AttributePathParams::Flags::kListIndexValid))
    {
        attributePath.ListIndex(aAttributePathParams.mListIndex);
    }

    attributePath.NodeId(aAttributePathParams.mNodeId)
        .ClusterId(aAttributePathParams.mClusterId)
        .EndpointId(aAttributePathParams.mEndpointId)
        .EndOfAttributePath();

    return attributePath.GetError();
}

CHIP_ERROR WriteClient::FinalizeMessage(System::PacketBufferHandle & aPacket)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    AttributeDataList::Builder attributeDataListBuilder;
    VerifyOrExit(mState == State::AddAttribute, err = CHIP_ERROR_INCORRECT_STATE);
    attributeDataListBuilder = mWriteRequestBuilder.GetAttributeDataListBuilder().EndOfAttributeDataList();
    err                      = attributeDataListBuilder.GetError();
    SuccessOrExit(err);

    mWriteRequestBuilder.EndOfWriteRequest();
    err = mWriteRequestBuilder.GetError();
    SuccessOrExit(err);

    err = mMessageWriter.Finalize(&aPacket);
    SuccessOrExit(err);

exit:
    return err;
}

const char * WriteClient::GetStateStr() const
{
#if CHIP_DETAIL_LOGGING
    switch (mState)
    {
    case State::Uninitialized:
        return "Uninitialized";

    case State::Initialized:
        return "Initialized";

    case State::AddAttribute:
        return "AddAttribute";

    case State::AwaitingResponse:
        return "AwaitingResponse";
    }
#endif // CHIP_DETAIL_LOGGING
    return "N/A";
}

void WriteClient::MoveToState(const State aTargetState)
{
    mState = aTargetState;
    ChipLogDetail(DataManagement, "WriteClient moving to [%10.10s]", GetStateStr());
}

void WriteClient::ClearState()
{
    MoveToState(State::Uninitialized);
}

CHIP_ERROR WriteClient::SendWriteRequest(NodeId aNodeId, FabricIndex aFabricIndex, Optional<SessionHandle> apSecureSession,
                                         uint32_t timeout)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    System::PacketBufferHandle packet;

    VerifyOrExit(mState == State::AddAttribute, err = CHIP_ERROR_INCORRECT_STATE);

    err = FinalizeMessage(packet);
    SuccessOrExit(err);

    // Create a new exchange context.
    mpExchangeCtx = mpExchangeMgr->NewContext(apSecureSession.ValueOr(SessionHandle(aNodeId, 0, 0, aFabricIndex)), this);
    VerifyOrExit(mpExchangeCtx != nullptr, err = CHIP_ERROR_NO_MEMORY);

    mpExchangeCtx->SetResponseTimeout(timeout);

    err = mpExchangeCtx->SendMessage(Protocols::InteractionModel::MsgType::WriteRequest, std::move(packet),
                                     Messaging::SendFlags(Messaging::SendMessageFlags::kExpectResponse));
    SuccessOrExit(err);

    MoveToState(State::AwaitingResponse);

exit:
    return err;
}

CHIP_ERROR WriteClient::OnMessageReceived(Messaging::ExchangeContext * apExchangeContext, const PayloadHeader & aPayloadHeader,
                                          System::PacketBufferHandle && aPayload)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    // Assert that the exchange context matches the client's current context.
    // This should never fail because even if SendWriteRequest is called
    // back-to-back, the second call will call Close() on the first exchange,
    // which clears the OnMessageReceived callback.
    VerifyOrExit(apExchangeContext == mpExchangeCtx, err = CHIP_ERROR_INCORRECT_STATE);

    // Verify that the message is an Write Response.
    // If not, close the exchange and free the payload.
    VerifyOrExit(aPayloadHeader.HasMessageType(Protocols::InteractionModel::MsgType::WriteResponse),
                 err = CHIP_ERROR_INVALID_MESSAGE_TYPE);

    SuccessOrExit(err = ProcessWriteResponseMessage(std::move(aPayload)));

exit:
    if (mpCallback != nullptr)
    {
        if (err != CHIP_NO_ERROR)
        {
            mpCallback->OnError(this, Protocols::InteractionModel::Status::Failure, err);
        }
    }

    Close();
    return CHIP_NO_ERROR;
}

void WriteClient::OnResponseTimeout(Messaging::ExchangeContext * apExchangeContext)
{
    ChipLogProgress(DataManagement, "Time out! failed to receive write response from Exchange: " ChipLogFormatExchange,
                    ChipLogValueExchange(apExchangeContext));

    if (mpCallback != nullptr)
    {
        mpCallback->OnError(this, Protocols::InteractionModel::Status::Failure, CHIP_ERROR_TIMEOUT);
    }
    Close();
}

CHIP_ERROR WriteClient::ProcessAttributeStatusElement(AttributeStatusElement::Parser & aAttributeStatusElement)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    AttributePath::Parser attributePath;
    Protocols::SecureChannel::GeneralStatusCode generalCode = Protocols::SecureChannel::GeneralStatusCode::kSuccess;
    uint32_t protocolId                                     = 0;
    uint16_t protocolCode                                   = 0;
    StatusElement::Parser statusElementParser;
    AttributePathParams attributePathParams;

    mAttributeStatusIndex++;
    err = aAttributeStatusElement.GetAttributePath(&attributePath);
    SuccessOrExit(err);
    err = attributePath.GetNodeId(&(attributePathParams.mNodeId));
    SuccessOrExit(err);
    err = attributePath.GetClusterId(&(attributePathParams.mClusterId));
    SuccessOrExit(err);
    err = attributePath.GetEndpointId(&(attributePathParams.mEndpointId));
    SuccessOrExit(err);

    err = attributePath.GetFieldId(&(attributePathParams.mFieldId));
    if (CHIP_NO_ERROR == err)
    {
        attributePathParams.mFlags.Set(AttributePathParams::Flags::kFieldIdValid);
    }
    else if (CHIP_END_OF_TLV == err)
    {
        err = CHIP_NO_ERROR;
    }
    SuccessOrExit(err);

    err = attributePath.GetListIndex(&(attributePathParams.mListIndex));
    if (CHIP_NO_ERROR == err)
    {
        VerifyOrExit(attributePathParams.mFlags.Has(AttributePathParams::Flags::kFieldIdValid),
                     err = CHIP_ERROR_IM_MALFORMED_ATTRIBUTE_PATH);
        attributePathParams.mFlags.Set(AttributePathParams::Flags::kListIndexValid);
    }

    SuccessOrExit(err = aAttributeStatusElement.GetStatusElement(&(statusElementParser)));
    SuccessOrExit(err = statusElementParser.DecodeStatusElement(&generalCode, &protocolId, &protocolCode));
    VerifyOrExit(mpCallback != nullptr, );

    if (protocolId == Protocols::InteractionModel::Id.ToFullyQualifiedSpecForm())
    {
        if (protocolCode == to_underlying(Protocols::InteractionModel::Status::Success))
        {
            mpCallback->OnSuccess(this, attributePathParams);
        }
        else
        {
            mpCallback->OnError(this, static_cast<Protocols::InteractionModel::Status>(protocolCode),
                                CHIP_ERROR_IM_STATUS_CODE_RECEIVED);
        }
    }
    else
    {
        mpCallback->OnError(this, Protocols::InteractionModel::Status::Failure, CHIP_ERROR_IM_STATUS_CODE_RECEIVED);
    }

exit:
    return err;
}

} // namespace app
} // namespace chip
