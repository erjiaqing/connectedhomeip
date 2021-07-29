/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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

#include <app/ReadHandler.h>

namespace chip {
namespace app {
/**
 *  @class SubscribeHandler
 *
 *  @brief The subscribe handler is responsible for processing a subscribe request, asking the attribute/event store
 *         for the relevant data, send a subscribe response, and further maintain the subscription.
 *
 */
class SubscribeHandler : public ReadHandler
{
public:
    CHIP_ERROR Init(InteractionModelDelegate * apDelegate) override;
    void Shutdown() override;
    CHIP_ERROR GetSubscriptionId(uint64_t & aSubscriptionId) override
    {
        aSubscriptionId = mSubscriptionId;
        return CHIP_NO_ERROR;
    }
    CHIP_ERROR OnStatusReport(Messaging::ExchangeContext * apExchangeContext, System::PacketBufferHandle && aPayload);
    CHIP_ERROR OnSubscribeRequest(Messaging::ExchangeContext * apExchangeContext, System::PacketBufferHandle && aPayload);
    bool IsSubscription() override { return true; };

private:
    CHIP_ERROR SendSubscribeResponse();
    CHIP_ERROR ProcessSubscribeRequest(System::PacketBufferHandle && aPayload);
    static void OnSubscribeTimerCallback(System::Layer * apSystemLayer, void * apAppState, CHIP_ERROR aError);
    CHIP_ERROR RefreshSubscribeSyncTimer(void);

    uint64_t mSubscriptionId = 0;
    uint16_t mFinalSyncIntervalSeconds = 0;
};
} // namespace app
} // namespace chip
