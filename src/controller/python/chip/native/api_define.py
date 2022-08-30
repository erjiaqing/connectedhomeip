#
#    Copyright (c) 2022 Project CHIP Authors
#    All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

from ctypes import *
from msilib.schema import Class
from chip.exceptions import ChipStackError
from dataclasses import dataclass
from typing import ClassVar, List, Callable


class VoidPointer(c_void_p):
    '''
    This prevents python auto-casting VoidPointer to integers and
    auto-casting 32/64 bit values to int/long respectively.
    Without this passing in VoidPointer does not see to work well
    for numbers in [0x80000000; 0xFFFFFFFF] (argument will be
    auto-cast to 64-bit negative)
    '''
    pass


class Callbacks:
    ''' This class is a namespace for various callback types used by underlying C function.
    '''
    IssueNOCChainCallbackPythonCallbackFunct = CFUNCTYPE(
        None, py_object, c_uint32, VoidPointer, c_size_t, VoidPointer, c_size_t, VoidPointer, c_size_t, VoidPointer, c_size_t, c_uint64)
    DevicePairingDelegate_OnPairingCompleteFunct = CFUNCTYPE(None, c_uint32)
    DevicePairingDelegate_OnCommissioningCompleteFunct = CFUNCTYPE(None, c_uint64, c_uint32)
    DevicePairingDelegate_OnCommissioningStatusUpdateFunct = CFUNCTYPE(None, c_uint64, c_uint8, c_uint32)
    DeviceAvailableFunct = CFUNCTYPE(None, VoidPointer, c_uint32)

    OnWriteResponseCallbackFunct = CFUNCTYPE(None, py_object, c_uint16, c_uint32, c_uint32, c_uint16)
    OnWriteErrorCallbackFunct = CFUNCTYPE(None, py_object, c_uint32)
    OnWriteDoneCallbackFunct = CFUNCTYPE(None, py_object)

    OnReadAttributeDataCallbackFunct = CFUNCTYPE(
        None, py_object, c_uint32, c_uint16, c_uint32, c_uint32, c_uint8, VoidPointer, c_size_t)
    OnSubscriptionEstablishedCallbackFunct = CFUNCTYPE(None, py_object, c_uint32)
    OnResubscriptionAttemptedCallbackFunct = CFUNCTYPE(None, py_object, c_uint32, c_uint32)
    OnReadEventDataCallbackFunct = CFUNCTYPE(None, py_object, c_uint16, c_uint32, c_uint32,
                                             c_uint64, c_uint8, c_uint64, c_uint8, VoidPointer, c_size_t, c_uint8)
    OnReadErrorCallbackFunct = CFUNCTYPE(None, py_object, c_uint32)
    OnReadDoneCallbackFunct = CFUNCTYPE(None, py_object)
    OnReportBeginCallbackFunct = CFUNCTYPE(None, py_object)
    OnReportEndCallbackFunct = CFUNCTYPE(None, py_object)

    OnCommandSenderResponseCallbackFunct = CFUNCTYPE(
        None, py_object, c_uint16, c_uint32, c_uint32, c_uint16, c_uint8, VoidPointer, c_uint32)
    OnCommandSenderErrorCallbackFunct = CFUNCTYPE(None, py_object, c_uint16, c_uint8, c_uint32)
    OnCommandSenderDoneCallbackFunct = CFUNCTYPE(None, py_object)

    IterateDiscoveredCommissionableNodesFunct = CFUNCTYPE(None, c_char_p, c_size_t)

    LogMessageFunct = CFUNCTYPE(None, c_int64, c_int64, c_char_p, c_uint8, c_char_p)
    ChipThreadTaskRunnerFunct = CFUNCTYPE(None, py_object)

    BleDeviceScannedCallback = CFUNCTYPE(None, py_object, c_char_p, c_uint16, c_uint16, c_uint16)
    BleScanDoneCallback = CFUNCTYPE(None, py_object)

# The following classes are namespaces for holding signatures of native functions. See the API define for more infomation.


@dataclass
class DeviceController:
    prefix: ClassVar[str] = 'pychip_DeviceController_'

    StackInit: Callable[[VoidPointer], ChipStackError]
    StackShutdown: Callable[[], ChipStackError]
    PostTaskOnChipThread: Callable[[Callbacks.ChipThreadTaskRunnerFunct, py_object], ChipStackError]

    CloseBleConnection: Callable[[VoidPointer], ChipStackError]
    CloseSession: Callable[[VoidPointer, c_uint64], ChipStackError]
    Commission: Callable[[VoidPointer, c_uint64], ChipStackError]
    ConnectBLE: Callable[[VoidPointer, c_uint16, c_uint32, c_uint64], ChipStackError]
    ConnectIP: Callable[[VoidPointer, c_char_p, c_uint32, c_uint64], ChipStackError]
    ConnectWithCode: Callable[[VoidPointer, c_char_p, c_uint64], ChipStackError]
    DeleteDeviceController: Callable[[VoidPointer], ChipStackError]
    HasDiscoveredCommissionableNode: Callable[[VoidPointer], c_bool]
    DiscoverAllCommissionableNodes: Callable[[VoidPointer], ChipStackError]
    DiscoverCommissionableNodes: Callable[[VoidPointer, c_uint8, c_char_p], ChipStackError]
    DiscoverCommissionableNodesCommissioningEnabled: Callable[[VoidPointer], ChipStackError]
    DiscoverCommissionableNodesDeviceType: Callable[[VoidPointer, c_uint16], ChipStackError]
    DiscoverCommissionableNodesLongDiscriminator: Callable[[VoidPointer, c_uint16], ChipStackError]
    DiscoverCommissionableNodesShortDiscriminator: Callable[[VoidPointer, c_uint16], ChipStackError]
    DiscoverCommissionableNodesVendor: Callable[[VoidPointer, c_uint16], ChipStackError]
    IterateDiscoveredCommissionableNodes: Callable[[VoidPointer, Callbacks.IterateDiscoveredCommissionableNodesFunct], None]
    EstablishPASESessionIP: Callable[[VoidPointer, c_char_p, c_uint32, c_uint64], ChipStackError]
    GetAddressAndPort: Callable[[VoidPointer, c_uint64, c_char_p, c_uint64, POINTER(c_uint16)], ChipStackError]
    GetCompressedFabricId: Callable[[VoidPointer, POINTER(c_uint64)], ChipStackError]
    GetFabricId: Callable[[VoidPointer, POINTER(c_uint64)], ChipStackError]
    GetIPForDiscoveredDevice: Callable[[VoidPointer, c_int, c_char_p, c_uint32], c_bool]
    IssueNOCChain: Callable[[VoidPointer, py_object, c_char_p, c_size_t, c_uint64], ChipStackError]
    OnNetworkCommission: Callable[[VoidPointer, c_uint64, c_uint32, c_uint8, c_char_p], ChipStackError]
    OpenCommissioningWindow: Callable[[VoidPointer, c_uint64, c_uint16, c_uint32, c_uint16, c_uint8], ChipStackError]
    PrintDiscoveredDevices: Callable[[VoidPointer], None]
    SetIssueNOCChainCallbackPythonCallback: Callable[[Callbacks.IssueNOCChainCallbackPythonCallbackFunct], None]
    SetThreadOperationalDataset: Callable[[c_char_p, c_uint32], ChipStackError]
    SetWiFiCredentials: Callable[[c_char_p, c_char_p], ChipStackError]


@dataclass
class ScriptDevicePairingDelegate:
    prefix: ClassVar[str] = 'pychip_ScriptDevicePairingDelegate_'

    SetCommissioningCompleteCallback: Callable[[
        VoidPointer, Callbacks.DevicePairingDelegate_OnCommissioningCompleteFunct], ChipStackError]
    SetCommissioningStatusUpdateCallback: Callable[[
        VoidPointer, Callbacks.DevicePairingDelegate_OnCommissioningStatusUpdateFunct], ChipStackError]
    SetKeyExchangeCallback: Callable[[VoidPointer, Callbacks.DevicePairingDelegate_OnPairingCompleteFunct], ChipStackError]


@dataclass
class OpCreds:
    prefix: ClassVar[str] = 'pychip_OpCreds_'

    AllocateController: Callable[[VoidPointer, POINTER(VoidPointer), c_uint64, c_uint64,
                                  c_uint16, c_char_p, c_bool, POINTER(c_uint32), c_uint32], ChipStackError]
    FreeDelegate: Callable[[VoidPointer], None]
    InitializeDelegate: Callable[[py_object, c_uint32, VoidPointer], VoidPointer]
    SetMaximallyLargeCertsUsed: Callable[[VoidPointer, c_bool], ChipStackError]


@dataclass
class DeviceProxy:
    prefix: ClassVar[str] = 'pychip_DeviceProxy_'

    ComputeRoundTripTimeout: Callable[[VoidPointer, c_uint32], c_uint32]


@dataclass
class CommissionableNodeController:
    prefix: ClassVar[str] = 'pychip_CommissionableNodeController_'

    NewController: Callable[[POINTER(VoidPointer)], ChipStackError]
    DeleteController: Callable[[VoidPointer], ChipStackError]
    DiscoverCommissioners: Callable[[VoidPointer], ChipStackError]
    PrintDiscoveredCommissioners: Callable[[VoidPointer], None]


@dataclass
class InteractionModel:
    prefix: ClassVar[str] = 'pychip_'

    CommandSender_InitCallbacks: Callbacks[[Callbacks.OnCommandSenderResponseCallbackFunct,
                                            Callbacks.OnCommandSenderErrorCallbackFunct, Callbacks.OnCommandSenderDoneCallbackFunct], None]
    CommandSender_SendCommand: Callbacks[[py_object, VoidPointer, c_uint16,
                                          c_uint32, c_uint32, c_char_p, c_size_t, c_uint16], ChipStackError]

    ReadClient_Abort: Callbacks[[VoidPointer, VoidPointer], None]
    ReadClient_InitCallbacks: Callbacks[[Callbacks.OnReadAttributeDataCallbackFunct, Callbacks.OnReadEventDataCallbackFunct, Callbacks.OnSubscriptionEstablishedCallbackFunct,
                                         Callbacks.OnResubscriptionAttemptedCallbackFunct, Callbacks.OnReadErrorCallbackFunct, Callbacks.OnReadDoneCallbackFunct, Callbacks.OnReportBeginCallbackFunct, Callbacks.OnReportEndCallbackFunct], None]
    ReadClient_OverrideLivenessTimeout: Callbacks[[VoidPointer, c_uint32], None]
    ReadClient_Read: Callable[..., ChipStackError]

    WriteClient_InitCallbacks: Callbacks[[Callbacks.OnWriteResponseCallbackFunct,
                                          Callbacks.OnWriteErrorCallbackFunct, Callbacks.OnWriteDoneCallbackFunct], None]
    WriteClient_WriteAttributes: Callable[..., ChipStackError]


@dataclass
class Stack:
    prefix: ClassVar[str] = 'pychip_Stack_'

    StatusReportToString: Callable[[c_uint32, c_uint16], c_char_p]
    ErrorToString: Callable[[c_uint32], c_char_p]
    SetLogFunct: Callable[[Callbacks.LogMessageFunct], ChipStackError]


@dataclass
class Ble:
    prefix: ClassVar[str] = 'pychip_ble_'

    adapter_list_new: Callable[[], VoidPointer]
    adapter_list_next: Callable[[VoidPointer], c_bool]
    adapter_list_get_index: Callable[[VoidPointer], c_uint32]
    adapter_list_get_address: Callable[[VoidPointer], c_char_p]
    adapter_list_get_alias: Callable[[VoidPointer], c_char_p]
    adapter_list_get_name: Callable[[VoidPointer], c_char_p]
    adapter_list_is_powered: Callable[[VoidPointer], c_bool]
    adapter_list_delete: Callable[[VoidPointer], None]
    adapter_list_get_raw_adapter: Callable[[VoidPointer], VoidPointer]
    start_scanning: Callable[[py_object, VoidPointer, c_uint32,
                              Callbacks.BleDeviceScannedCallback, Callbacks.BleScanDoneCallback], VoidPointer]


@dataclass
class ApiDefine:
    ''' This class (and the classes above) defines the native functions used by the Python module.

    chip.native.Api should be the only instance of this class, and it will be initialized when calling `chip.native.Init()`

    This class (and the above classes) contains three parts:
    - prefix           Defines the prefix of the function names
    - Name: ClassName  Defines a subnamespace for a set of functions
    - Name: Signature  Defines a single API, with the help of type hints, in the format of
                       Callable[[ArgTypes], RetType], RetType can be:
      - ChipStackError: The underlying function returns an Chi pError code, and Python should raise an exception if it is non-zero.
      - None:           The underlying function is a void function.
      - sone type:      The underlying function returns value of the given type.

    For example, if the prefix is `pychip_`, and we have a member:

    ```
    FooBar: Callable[[VoidPointer, c_uint64], ChipStackError]
    ```

    Means we have the following native function:
    ```
    ChipError::StorageType pychip_FooBar(void*, uint64_t)
    ```

    Calling FooBar will return `None` if `pychip_FooBar` returns 0, but will raise an exception if `pychip_FooBar` returns non-zero values.
    '''

    prefix: ClassVar[str] = 'pychip_'

    Ble: Ble
    DeviceController: DeviceController
    DeviceProxy: DeviceProxy
    InteractionModel: InteractionModel
    OpCreds: OpCreds
    ScriptDevicePairingDelegate: ScriptDevicePairingDelegate
    Stack: Stack

    CommonStackShutdown: Callable[[], None]
    ExpireSessions: Callable[[VoidPointer, c_uint64], ChipStackError]
    FreeOperationalDeviceProxy: Callable[[VoidPointer], ChipStackError]
    GetCommandSenderHandle: Callable[[VoidPointer], c_uint64]
    GetConnectedDeviceByNodeId: Callable[[VoidPointer, c_uint64, Callbacks.DeviceAvailableFunct], ChipStackError]
    GetDeviceBeingCommissioned: Callable[[VoidPointer, c_uint64, VoidPointer], ChipStackError]
    GetLocalSessionId: Callable[[VoidPointer, POINTER(c_uint16)], ChipStackError]
    GetNumSessionsToPeer: Callable[[VoidPointer, POINTER(c_uint32)], ChipStackError]
    ResetCommissioningTests: Callable[[], None]
    SetTestCommissionerSimulateFailureOnReport: Callable[[c_uint8], c_bool]
    SetTestCommissionerSimulateFailureOnStage: Callable[[c_uint8], c_bool]
    TestCommissionerUsed: Callable[[], c_bool]
    TestCommissioningCallbacks: Callable[[], None]
    TestPaseConnection: Callable[[c_uint64], None]
