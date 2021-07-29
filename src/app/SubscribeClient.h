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
 *      This file defines subscription client for a CHIP Interaction Data model
 *
 */

#pragma once

#include <app/ReadClient.h>

namespace chip {
namespace app {
/**
 *  @class SubscribeClient
 *
 *  @brief The subscribe client represents the initiator side of a Subscribe Interaction, and is responsible
 *  for generating one Subscribe Request for a particular set of attributes and/or events, and handling the Report Data response.
 *
 */
class SubscribeClient : public ReadClient
{
public:
    CHIP_ERROR Init(Messaging::ExchangeManager * apExchangeMgr, InteractionModelDelegate * apDelegate, intptr_t aAppIdentifier) override;
    /**
     *  Send a Subscribe Request.  There can be one Subscribe Request outstanding on a given SubscribeClient.
     *  If SendSubscribeRequest returns success, no more Subscribe Requests can be sent on this SubscribeClient
     *  until the corresponding InteractionModelDelegate::ReportProcessed or InteractionModelDelegate::ReportError
     *  call happens with guarantee.
     *
     *  @retval #others fail to send read request
     *  @retval #CHIP_NO_ERROR On success.
     */
    CHIP_ERROR SendSubscribeRequest();

    void ResetResubscribe();
    void EnableResubscribe(bool aEnable);
    bool IsSubscription() const override { return true; };
    virtual ~SubscribeClient() = default;

private:
    /**
     * Internal shutdown method that we use when we know what's going on with
     * our exchange and don't need to manually close it.
     */
    void ShutdownInternal() override;
    CHIP_ERROR ProcessReportData(System::PacketBufferHandle && aPayload) override;
    CHIP_ERROR SendStatusReport(CHIP_ERROR aError);
    CHIP_ERROR OnMessageReceived(Messaging::ExchangeContext * apExchangeContext, const PacketHeader & aPacketHeader,
                                 const PayloadHeader & aPayloadHeader, System::PacketBufferHandle && aPayload) override;
    CHIP_ERROR ProcessSubscribeResponse(System::PacketBufferHandle && aPayload);
    void OnResponseTimeout(Messaging::ExchangeContext * apExchangeContext) override;
    void SetRetryTimer();

    CHIP_ERROR RefreshLivenessCheckTimer();
    void CancelLivenessCheckTimer();
    static void OnLivenessTimeoutCallback(System::Layer * apSystemLayer, void * apAppState, CHIP_ERROR aError);
    static void OnReSubscribeTimerCallback(System::Layer * apSystemLayer, void * apAppState, CHIP_ERROR aError);

    bool IsValidSubscription(uint64_t & aSubscriptionId) override {
        return aSubscriptionId == mSubscriptionId;
    }
    void CancelResubscribe();

    bool mEnableResubscribe = false;
    uint32_t mRetryCounter = 0;
    uint16_t mFinalSyncIntervalSeconds = 0;
    uint64_t mSubscriptionId = 0;
};
}; // namespace app
}; // namespace chip
