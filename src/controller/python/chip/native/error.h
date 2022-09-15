/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
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

#include <lib/core/CHIPError.h>

struct PyChipError
{
    uint32_t mCode;
    uint32_t mLine;
    const char * mFile;

    PyChipError() : mCode(0), mLine(0), mFile(nullptr) {}
    PyChipError(const CHIP_ERROR & aChipError) :
        mCode(aChipError.AsInteger()),
#if CHIP_CONFIG_ERROR_SOURCE
        mLine(aChipError.GetLine()), mFile(aChipError.GetFile())
#else
        mLine(0), mFile(nullptr)
#endif
    {}

    bool operator==(const CHIP_ERROR & aOther) { return mCode == aOther.AsInteger(); }
};

static_assert(std::is_same<uint32_t, chip::ChipError::StorageType>::value, "python assumes CHIP_ERROR maps to c_uint32");
