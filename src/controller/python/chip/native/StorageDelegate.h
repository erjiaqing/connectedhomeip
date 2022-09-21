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

#include <map>
#include <string>

#include <controller/python/chip/native/Stack.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>

class PythonPersistentStorageDelegate;

struct pychip_StorageAdapter
{
};

namespace chip {

class PythonPersistentStorageDelegate : public PersistentStorageDelegate
{
public:
    PythonPersistentStorageDelegate() {}
    CHIP_ERROR SyncGetKeyValue(const char * key, void * buffer, uint16_t & size) override;
    CHIP_ERROR SyncSetKeyValue(const char * key, const void * value, uint16_t size) override;
    CHIP_ERROR SyncDeleteKeyValue(const char * key) override;

private:
    std::map<std::string, std::string> mStorage;
};

namespace Python {

class StorageAdapter : public PersistentStorageDelegate, public pychip_StorageAdapter
{
public:
    StorageAdapter(PyObject * context, pychip_Storage_SyncSetKeyValueCb setCb, pychip_Storage_SetGetKeyValueCb getCb,
                   pychip_Storage_SyncDeleteKeyValueCb deleteCb)
    {
        mSetKeyCb    = setCb;
        mGetKeyCb    = getCb;
        mDeleteKeyCb = deleteCb;
        mContext     = context;
    }

    CHIP_ERROR SyncGetKeyValue(const char * key, void * buffer, uint16_t & size) override;
    CHIP_ERROR SyncSetKeyValue(const char * key, const void * value, uint16_t size) override;
    CHIP_ERROR SyncDeleteKeyValue(const char * key) override;

private:
    pychip_Storage_SyncSetKeyValueCb mSetKeyCb;
    pychip_Storage_SetGetKeyValueCb mGetKeyCb;
    pychip_Storage_SyncDeleteKeyValueCb mDeleteKeyCb;
    PyObject * mContext;
};

} // namespace Python
} // namespace chip
