'''
   Copyright (c) 2021 Project CHIP Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
'''

from construct import Struct, Int64ul, Int32ul, Int16ul, Int8ul
from ctypes import CFUNCTYPE, c_char_p, c_uint16, c_void_p, c_size_t, c_uint32, c_uint64, c_uint8
from dataclasses import dataclass
from typing import *
import ctypes
import chip.native
import threading
import chip.exceptions
import chip.tlv as chiptlv

IMCommandStatus = Struct(
    "ProtocolId" /  Int32ul,
    "ProtocolCode" / Int16ul,
    "EndpointId" / Int8ul,
    "ClusterId" / Int16ul,
    "CommandId" / Int8ul,
    "CommandIndex" / Int8ul,
)

IMClusterAttributeInfo = Struct(
    "ClusterId" / Int16ul,
    "AttributeId" / Int16ul,
    "EndpointId" / Int8ul,
)

@dataclass
class ReadCallback:
    OnData: Callable[[IMClusterAttributeInfo, Any, int], None]
    OnEnd: Callable[[int], None]

# typedef void (*PythonInteractionModelDelegate_OnCommandResponseStatusCodeReceivedFunct)(uint64_t commandSenderPtr,
#                                                                                         void * commandStatusBuf);
# typedef void (*PythonInteractionModelDelegate_OnCommandResponseProtocolErrorFunct)(uint64_t commandSenderPtr, uint8_t commandIndex);
# typedef void (*PythonInteractionModelDelegate_OnCommandResponseFunct)(uint64_t commandSenderPtr, uint32_t error);
_OnCommandResponseStatusCodeReceivedFunct = CFUNCTYPE(None, c_uint64, c_void_p, c_uint32)
_OnCommandResponseProtocolErrorFunct = CFUNCTYPE(None, c_uint64, c_uint8)
_OnCommandResponseFunct = CFUNCTYPE(None, c_uint64, c_uint32)

# typedef void (*PythonInteractionModelDelegate_OnReportData)(intptr_t applicationIndex, const char * apClusterAttributeInfoBuf,
#                                                             size_t aClusterAttributeInfoBufLen, const uint8_t * apDataBuf,
#                                                             size_t aDataBufLength, uint8_t status);
# typedef void (*PythonInteractionModelDelegate_OnReportEnd)(intptr_t applicationIndex, CHIP_ERROR aError);
_OnReportDataFunct = CFUNCTYPE(None, c_uint64, c_void_p, c_size_t, c_void_p, c_size_t, c_uint8)
_OnReportEndFunct = CFUNCTYPE(None, c_uint64, c_uint32)

_commandStatusDict = dict()
_commandIndexStatusDict = dict()
_commandStatusLock = threading.RLock()
_commandStatusCV = threading.Condition(_commandStatusLock)

_attributeCallbackDict = dict()
_attributeCallbackDictLock = threading.RLock()

# A placeholder commandHandle, will be removed once we decouple CommandSender with CHIPClusters
PLACEHOLDER_COMMAND_HANDLE = 1

_next_read_handle = 256

def GetReadHandle()->int:
    global _next_read_handle
    with _attributeCallbackDictLock:
        value = _next_read_handle
        _next_read_handle += 1
        return value

def _GetCommandStatus(commandHandle: int):
    with _commandStatusLock:
        return _commandStatusDict.get(commandHandle, None)

def _GetCommandIndexStatus(commandHandle: int, commandIndex: int):
    with _commandStatusLock:
        indexDict = _commandIndexStatusDict.get(commandHandle, {})
        return indexDict.get(commandIndex, None)

def _SetCommandStatus(commandHandle: int, val):
    with _commandStatusLock:
        _commandStatusDict[commandHandle] = val
        _commandStatusCV.notify_all()

def _SetCommandIndexStatus(commandHandle: int, commandIndex: int, status):
    with _commandStatusLock:
        print("SetCommandIndexStatus commandHandle={} commandIndex={}".format(commandHandle, commandIndex))
        indexDict = _commandIndexStatusDict.get(commandHandle, {})
        indexDict[commandIndex] = status
        _commandIndexStatusDict[commandHandle] = indexDict

@_OnCommandResponseStatusCodeReceivedFunct
def _OnCommandResponseStatusCodeReceived(commandHandle: int, IMCommandStatusBuf, IMCommandStatusBufLen):
    status = IMCommandStatus.parse(ctypes.string_at(IMCommandStatusBuf, IMCommandStatusBufLen))
    _SetCommandIndexStatus(PLACEHOLDER_COMMAND_HANDLE, status["CommandIndex"], status)

@_OnCommandResponseProtocolErrorFunct
def _OnCommandResponseProtocolError(commandHandle: int, errorcode: int):
    pass

@_OnCommandResponseFunct
def _OnCommandResponse(commandHandle: int, errorcode: int):
    _SetCommandStatus(PLACEHOLDER_COMMAND_HANDLE, errorcode)

@_OnReportDataFunct
def _OnReportData(handle: int, attributeInfoBuf, attributeInfoBufLen, dataTLVBuf, dataTLVBufLen, status: int):
    callback = None
    with _attributeCallbackDictLock:
        callback = _attributeCallbackDict.get(handle, None)
    attributeInfo = IMClusterAttributeInfo.parse(ctypes.string_at(attributeInfoBuf, attributeInfoBufLen))
    attributeData = chiptlv.TLVReader(ctypes.string_at(dataTLVBuf, dataTLVBufLen)).get()
    if callback is not None:
        callback.OnData(attributeInfo, attributeData, status)

@_OnReportEndFunct
def _OnReportEnd(handle: int, error: int):
    callback = None
    with _attributeCallbackDictLock:
        callback = _attributeCallbackDict.pop(handle, None)
    callback.OnEnd(error)

def RegisterReadCallback(handle: int, callback: ReadCallback):
    if handle < 256:
        raise ValueError("Handle to register read callback must be larger than 256")
    with _attributeCallbackDictLock:
        if _attributeCallbackDict.get(handle, None) is not None:
            raise LookupError("The callback for {} already exists.".format(handle))
        _attributeCallbackDict[handle] = callback

def CancelReadCallback(handle: int):
    with _attributeCallbackDictLock:
        _attributeCallbackDict.pop(handle, None)

def InitIMDelegate():
    handle = chip.native.GetLibraryHandle()
    if not handle.pychip_InteractionModelDelegate_SetCommandResponseStatusCallback.argtypes:
        setter = chip.native.NativeLibraryHandleMethodArguments(handle)
        setter.Set("pychip_InteractionModelDelegate_SetCommandResponseStatusCallback", None, [_OnCommandResponseStatusCodeReceivedFunct])
        setter.Set("pychip_InteractionModelDelegate_SetCommandResponseProtocolErrorCallback", None, [_OnCommandResponseProtocolErrorFunct])
        setter.Set("pychip_InteractionModelDelegate_SetCommandResponseErrorCallback", None, [_OnCommandResponseFunct])
        setter.Set("pychip_InteractionModel_GetCommandSenderHandle", c_uint32, [ctypes.POINTER(c_uint64)])
        setter.Set("pychip_InteractionModelDelegate_SetOnReportDataCallback", None, [_OnReportDataFunct])
        setter.Set("pychip_InteractionModelDelegate_SetOnReportEndCallback", None, [_OnReportEndFunct])

        # CHIP_ERROR pychip_im_SendReadRequest(NodeId aNodeId, Transport::AdminId aAdminId, ClusterAttributeInfo * apAttributes, size_t apAttributesLen, uint64_t aHandle)
        setter.Set("pychip_im_SendReadRequest", c_uint32, [c_uint64, c_uint16, c_void_p, c_size_t, c_uint64])

        handle.pychip_InteractionModelDelegate_SetCommandResponseStatusCallback(_OnCommandResponseStatusCodeReceived)
        handle.pychip_InteractionModelDelegate_SetCommandResponseProtocolErrorCallback(_OnCommandResponseProtocolError)
        handle.pychip_InteractionModelDelegate_SetCommandResponseErrorCallback(_OnCommandResponse)
        handle.pychip_InteractionModelDelegate_SetOnReportDataCallback(_OnReportData)
        handle.pychip_InteractionModelDelegate_SetOnReportEndCallback(_OnReportEnd)

def ClearCommandStatus(commandHandle: int):
    """
    Clear internal state and prepare for next command, should be called before sending commands.
    """
    with _commandStatusLock:
        _SetCommandStatus(commandHandle, None)
        _commandIndexStatusDict[commandHandle] = {}

def WaitCommandStatus(commandHandle: int):
    """
    Wait for response from device, returns error code.
    ClearCommandStatus should be called before sending command or it will
    return result from last command sent.
    """
    # commandHandle is null, means we are not using IM
    # Note: This should be an error after we fully switched to IM.
    if commandHandle == 0:
        return None
    with _commandStatusCV:
        ret = _GetCommandStatus(commandHandle)
        while ret is None:
            _commandStatusCV.wait()
            ret = _GetCommandStatus(commandHandle)
        return ret

def WaitCommandIndexStatus(commandHandle: int, commandIndex: int):
    """
    Wait for response of particular command from device, returns error code and struct of response info.
    When device returns a command instead of command status, the response info is None.
    ClearCommandStatus should be called before sending command or it will
    return result from last command sent.
    """
    # commandHandle is null, means we are not using IM
    # Note: This should be an error after we fully switched to IM.
    if commandHandle == 0:
        return (0, None)
    err = WaitCommandStatus(commandHandle)
    return (err, _GetCommandIndexStatus(commandHandle, commandIndex))

def GetCommandSenderHandle()->int:
    handle = chip.native.GetLibraryHandle()
    resPointer = c_uint64()
    res = handle.pychip_InteractionModel_GetCommandSenderHandle(ctypes.pointer(resPointer))
    if res != 0:
        raise chip.exceptions.ChipStackError(res)
    ClearCommandStatus(resPointer.value)
    return resPointer.value
