#
#    Copyright (c) 2020 Project CHIP Authors
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

from asyncio import threads
from asyncio.futures import Future
import ctypes
from dataclasses import dataclass, asdict
from typing import List, Dict, Any, Mapping, Type
import asyncio
import threading
from ctypes import CFUNCTYPE, c_char, c_char_p, c_size_t, c_void_p, c_uint32, c_uint64, c_uint8, c_uint16, c_ssize_t, py_object


from . import ClusterObjects
import chip
from chip import tlv
from chip import exceptions as ChipExceptions
from chip.interaction_model import exceptions as IMExceptions


@dataclass
class CommandPath:
    EndpointId: int
    ClusterId: int
    CommandId: int


class AsyncCommandTransaction:
    def __init__(self, future: Future, eventLoop, expectType: Type):
        self._event_loop = eventLoop
        self._future = future
        self._expect_type = expectType

    def _handleResponse(self, response: bytes):
        if self._expect_type:
            try:
                self._future.set_result(self._expect_type.FromTLV(response))
            except Exception as ex:
                self._handleError(
                    IMExceptions.InteractionModelState.Failure, 0, ex)
        else:
            self._future.set_result(None)

    def handleResponse(self, path: CommandPath, response: bytes):
        self._event_loop.call_soon_threadsafe(
            self._handleResponse, response)

    def _handleError(self, imError: int, chipError: int, exception: Exception):
        if exception:
            self._future.set_exception(exception)
        elif chipError != 0:
            self._future.set_exception(
                ChipExceptions.ChipStackError(chipError))
        else:
            try:
                self._future.set_exception(
                    IMExceptions.InteractionModelError(IMExceptions.InteractionModelState(imError)))
            except:
                self._future.set_exception(IMExceptions.InteractionModelError(
                    IMExceptions.InteractionModelState.Failure))
        pass

    def handleError(self, imError: int, chipError: int):
        self._event_loop.call_soon_threadsafe(
            self._handleError, imError, chipError, None
        )


'''
void pychip_CommandSender_SendCommand(void * appContext, Controller::Device * device, chip::EndpointId endpointId,
                                      chip::ClusterId clusterId, chip::CommandId commandId, const uint8_t * payload, size_t length);
using OnCommandSenderResponseCallback = void (*)(PyObject appContext, chip::EndpointId endpointId, chip::ClusterId clusterId,
                                                 chip::CommandId commandId, const uint8_t * payload, size_t length);
using OnCommandSenderErrorCallback    = void (*)(PyObject appContext,
                                              std::underlying_type_t<Protocols::InteractionModel::Status> imstatus,
                                              uint32_t chiperror);
'''

_OnCommandSenderResponseCallbackFunct = CFUNCTYPE(
    None, py_object, c_uint16, c_uint32, c_uint32, c_void_p, c_uint32)
_OnCommandSenderErrorCallbackFunct = CFUNCTYPE(
    None, py_object, c_uint16, c_uint32)
_OnCommandSenderFinalCallbackFunct = CFUNCTYPE(
    None, py_object)


@_OnCommandSenderResponseCallbackFunct
def _OnCommandSenderResponseCallback(closure, endpoint: int, cluster: int, command: int, payload, size):
    data = ctypes.string_at(payload, size)
    closure.handleResponse(CommandPath(endpoint, cluster, command), data[:])


@_OnCommandSenderErrorCallbackFunct
def _OnCommandSenderErrorCallback(closure, imerror: int, chiperror: int):
    closure.handleError(imerror, chiperror)


@_OnCommandSenderFinalCallbackFunct
def _OnCommandSenderFinalCallback(closure):
    ctypes.pythonapi.Py_DecRef(ctypes.py_object(closure))


def SendCommand(future: Future, eventLoop, expectedType: Type, device, commandPath: CommandPath, payload: ClusterObjects.ClusterObject):
    handle = chip.native.GetLibraryHandle()
    transaction = AsyncCommandTransaction(future, eventLoop, expectedType)

    payloadTLV = payload.ToTLV()
    ctypes.pythonapi.Py_IncRef(ctypes.py_object(transaction))
    handle.pychip_CommandSender_SendCommand(ctypes.py_object(
        transaction), device, commandPath.EndpointId, commandPath.ClusterId, commandPath.CommandId, payloadTLV, len(payloadTLV))


def Init():
    handle = chip.native.GetLibraryHandle()

    # Uses one of the type decorators as an indicator for everything being
    # initialized.
    if not handle.pychip_CommandSender_SendCommand.argtypes:
        setter = chip.native.NativeLibraryHandleMethodArguments(handle)

        setter.Set('pychip_CommandSender_SendCommand',
                   None, [py_object, c_void_p, c_uint16, c_uint32, c_uint32, c_char_p, c_size_t])
        setter.Set('pychip_CommandSender_InitCallbacks', None, [
                   _OnCommandSenderResponseCallbackFunct, _OnCommandSenderErrorCallbackFunct, _OnCommandSenderFinalCallbackFunct])

    handle.pychip_CommandSender_InitCallbacks(
        _OnCommandSenderResponseCallback, _OnCommandSenderErrorCallback, _OnCommandSenderFinalCallback)


_deviceController = None


def SetDeviceController(deviceCtrl):
    global _deviceController
    _deviceController = deviceCtrl


def GetDeviceController():
    global _deviceController
    return _deviceController
