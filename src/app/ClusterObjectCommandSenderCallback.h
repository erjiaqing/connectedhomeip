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

#pragma once

#include <app/CommandSender.h>
#include <app/common/StatusElement.h>
#include <app/data-model/Decode.h>

#include <functional>

namespace chip {
namespace app {

template <typename ClusterObjectT>
class ClusterObjectCommandSenderCallback final : public CommandSender::Delegate
{
public:
    template <typename ClusterObjectT2>
    using OnResponseCallbackType<ClusterObjectT2> =
        std::function<void(const CommandSender *, const CommandPath::Type &, const ClusterObjectT2 &)>;

    template <>
    using OnResponseCallbackType<void> = std::function<void(const CommandSender *, const CommandPath::Type &)>;

    using OnErrorCallbackTyoe =
        std::function<void(const CommandSender *, Protocols::InteractionModel::Status aIMStatus, CHIP_ERROR aError)>;

    using OnFinalCallbackType = std::function<void(CommandSender *, ClusterObjectCommandSenderCallback<ClusterObjectT> *)>;

    ClusterObjectCommandSenderCallback(OnResponseCallbackType<ClusterObjectT> aOnResponse, OnErrorCallbackTyoe aOnError,
                                       OnFinalCallbackType aOnFinal) :
        mOnResponse(aOnResponse),
        mOnError(aOnError), mOnFinal(aOnFinal)
    {}

    void OnResponse(const CommandSender * apCommandSender, const CommandPath::Type & aCommandPath, TLV::TLVReader & aReader);
    void OnError(const CommandSender * apCommandSender, Protocols::InteractionModel::Status aIMStatus, CHIP_ERROR aError)
    {
        mOnError(apCommandSender, aIMStatus, aError);
    }
    void OnFinal(CommandSender * apCommandSender) { mOnFinal(apCommandSender, this); }

private:
    OnResponseCallbackType<ClusterObjectT> mOnResponse;
    OnErrorCallbackTyoe mOnError;
    OnFinalCallbackType mOnFinal;
};

template <typename ClusterObjectT>
void ClusterObjectCommandSenderCallback<ClusterObjectT>::OnResponse(const CommandSender * apCommandSender,
                                                                    const CommandPath::Type & aCommandPath,
                                                                    TLV::TLVReader & aReader)
{
    ClusterObjectT response;
    CHIP_ERROR err = DataModel::Decode(aReader, response);
    if (err != CHIP_NO_ERROR)
    {
        mOnError(err);
        return;
    }
    mOnResponse(apCommandSender, aCommandPath, response);
}

template <>
void ClusterObjectCommandSenderCallback<void>::OnResponse(const CommandSender * apCommandSender,
                                                          const CommandPath::Type & aCommandPath, TLV::TLVReader & aReader);

} // namespace app
} // namespace chip
