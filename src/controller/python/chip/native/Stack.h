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

/**
 * This file contains the C APIs defined for Python.
 */

#pragma once

#include "PyChipError.h"

#ifdef __cplusplus
extern "C" {
#endif

struct PyObject;
struct pychip_StorageAdapter;
struct pychip_Controller_DeviceCommissioner;

// Type alias
typedef uint64_t pychip_NodeId;

typedef void (*pychip_Storage_SyncSetKeyValueCb)(PyObject * context, const char * key, const void * value, uint16_t size);
typedef void (*pychip_Storage_SetGetKeyValueCb)(PyObject * context, const char * key, char * value, uint16_t * size,
                                                bool * isFound);
typedef void (*pychip_Storage_SyncDeleteKeyValueCb)(PyObject * context, const char * key);

// Util
void pychip_FormatError(PyChipError * apError, char * apBuf, uint32_t aBufSize);

// Stack
struct pychip_CommonStackInitParams;
PyChipError pychip_StackInit(pychip_StorageAdapter * storageAdapter, bool enableServerInteractions);
PyChipError pychip_StackShutdown();
PyChipError pychip_CommonStackInit(const pychip_CommonStackInitParams * aParams);
void pychip_CommonStackShutdown();
void pychip_CauseCrash();

// Storage
pychip_StorageAdapter * pychip_Storage_InitializeStorageAdapter(PyObject * context, pychip_Storage_SyncSetKeyValueCb setCb,
                                                                pychip_Storage_SetGetKeyValueCb getCb,
                                                                pychip_Storage_SyncDeleteKeyValueCb deleteCb);
void pychip_Storage_ShutdownAdapter(pychip_StorageAdapter * storageAdapter);

// Controller
PyChipError pychip_DeviceController_NewDeviceController(pychip_Controller_DeviceCommissioner ** outDevCtrl,
                                                        pychip_NodeId localDeviceId, bool useTestCommissioner);

#ifdef __cplusplus
}
#endif
