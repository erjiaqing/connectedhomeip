/*
 *
 *    Copyright (c) 2020-2021 Project CHIP Authors
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

namespace chip {
namespace app {
namespace DataModel {

/*
 * Dedicated type for list<T> that is at its base, just a Span.
 *
 * Motivated by the need to create distinction between Lists that use Spans
 * vs. other data model types that use Spans (like octetstr). These have different
 * encodings. Consequently, there needs to be an actual C++ type distinction to ensure
 * correct specialization of the Encode/Decode methods.
 *
 */

template <typename T>
class IsFabricSensitive
{
private:
    typedef char trueType[1];
    typedef char falseType[2];

    template <typename Tp>
    static trueType & test(decltype(&Tp::FabricIndexMatch));
    template <typename Tp>
    static falseType & test(...);

public:
    static constexpr bool value = (sizeof(test<T>(0)) == sizeof(trueType));
};

} // namespace DataModel
} // namespace app
} // namespace chip
