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

from dataclasses import is_dataclass
import glob
import os
import ctypes
import platform
from types import NoneType
import typing
from typing import Callable, Type, Any
import construct
import inspect

from . import api_define
from chip.exceptions import ChipStackError

NATIVE_LIBRARY_BASE_NAME = "_ChipDeviceCtrl.so"

# The main API namespace
Api: api_define.ApiDefine = None
Callbacks = api_define.Callbacks


def _AllDirsToRoot(dir):
    """Return all parent paths of a directory."""
    dir = os.path.abspath(dir)
    while True:
        yield dir
        parent = os.path.dirname(dir)
        if parent == "" or parent == dir:
            break
        dir = parent


def FindNativeLibraryPath() -> str:
    """Find the native CHIP dll/so path."""

    scriptDir = os.path.dirname(os.path.abspath(__file__))

    # When properly installed in the chip package, the Chip Device Manager DLL will
    # be located in the package root directory, along side the package's
    # modules.
    dmDLLPath = os.path.join(
        os.path.dirname(scriptDir),  # file should be inside 'chip'
        NATIVE_LIBRARY_BASE_NAME)
    if os.path.exists(dmDLLPath):
        return dmDLLPath

    # For the convenience of developers, search the list of parent paths relative to the
    # running script looking for an CHIP build directory containing the Chip Device
    # Manager DLL. This makes it possible to import and use the ChipDeviceMgr module
    # directly from a built copy of the CHIP source tree.
    buildMachineGlob = "%s-*-%s*" % (platform.machine(),
                                     platform.system().lower())
    relDMDLLPathGlob = os.path.join(
        "build",
        buildMachineGlob,
        "src/controller/python/.libs",
        NATIVE_LIBRARY_BASE_NAME,
    )
    for dir in _AllDirsToRoot(scriptDir):
        dmDLLPathGlob = os.path.join(dir, relDMDLLPathGlob)
        for dmDLLPath in glob.glob(dmDLLPathGlob):
            if os.path.exists(dmDLLPath):
                return dmDLLPath

    raise Exception(
        "Unable to locate Chip Device Manager DLL (%s); expected location: %s" %
        (NATIVE_LIBRARY_BASE_NAME, scriptDir))


class NativeLibraryHandleMethodArguments:
    """Convenience wrapper to set native method argtype and restype for methods."""

    def __init__(self, handle):
        self.handle = handle

    def Set(self, methodName: str, resultType, argumentTypes: list):
        method = getattr(self.handle, methodName)
        method.restype = resultType
        method.argtype = argumentTypes


_nativeLibraryHandle = ctypes.CDLL(FindNativeLibraryPath())
_nativeLibraryInitialized = False


def _GetLibraryHandle(shouldInit: bool) -> ctypes.CDLL:
    """Get a memoized handle to the chip native code dll."""

    global _nativeLibraryHandle
    global _nativeLibraryInitialized

    if shouldInit and not _nativeLibraryInitialized:
        raise Exception("Common stack has not been initialized!")

    if not _nativeLibraryHandle.pychip_CommonStackInit.argtypes:
        setter = NativeLibraryHandleMethodArguments(_nativeLibraryHandle)
        setter.Set("pychip_CommonStackInit", ctypes.c_uint32, [ctypes.c_char_p])

    return _nativeLibraryHandle


def GetLibraryHandle():
    return _GetLibraryHandle(True)


def GetLastError(expected_err=None) -> ChipStackError:
    ''' Returns the exception object from the current thread.

    The function should be invoked from the function where the previous function is called.
    Users should use `CallNativeWithException` instead of calling this function by their own.
    '''
    return ChipStackError(expected_err)


def CallNativeWithException(func, *args):
    ''' Calling a function from Matter's native library, and raise an exception when it returns non-zero value.
    '''
    res = func(*args)
    if res != 0:
        raise GetLastError(expected_err=res)


def Init(bluetoothAdapter: int = None):
    global _nativeLibraryInitialized
    global Api

    CommonStackParams = construct.Struct(
        "BluetoothAdapterId" / construct.Int32ul,
    )
    params = CommonStackParams.parse(b'\x00' * CommonStackParams.sizeof())
    params.BluetoothAdapterId = bluetoothAdapter if bluetoothAdapter is not None else 0
    params = CommonStackParams.build(params)

    _GetLibraryHandle(False).pychip_CommonStackInit(ctypes.c_char_p(params))
    Api = _BuildApi(api_define.ApiDefine)
    _nativeLibraryInitialized = True


# Note: Below is the experimental API for calling functions and convert ChipError to throwing exceptions fluently.

def _GetNativeFunc(func: str):
    return getattr(_nativeLibraryHandle, func)


def _DmlibFunc(func: str, signature: Type[Callable]) -> Callable[..., Any]:
    ''' Wraps the func from Matter dynamic library into Python function.

    If ret_type is ChipStackError, calling it will raise an exception if the underlying function returns an non-zero error code.

    The signature is a Callable[[Type of Args], ReturnType], where the ReturnType can be:
    - None / NoneType: The underlying function returns nothing.
    - ChipStackError:  The underlying function returns an ChipError error code, and an exception should be raised if it is not zero,
    - Types:           The underlying returns value of corresponding type.
    '''
    arg_types, ret_type = typing.get_args(signature)

    c_ret_type = ret_type
    if ret_type == NoneType or ret_type is None:
        c_ret_type = None
    elif inspect.isclass(ret_type) and issubclass(ret_type, ChipStackError):
        c_ret_type = ctypes.c_uint32
    f = _GetNativeFunc(func)
    f.restype = c_ret_type
    if arg_types is not Ellipsis:
        f.argtype = arg_types

    def CallWithException(*args: arg_types) -> ret_type:
        CallNativeWithException(f, *args)

    def Call(*args: arg_types) -> ret_type:
        return f(*args)

    ret_func = Call
    if issubclass(ret_type, ChipStackError):
        ret_func = CallWithException

    ret_func.__name__ = func

    return ret_func


def _BuildApi(cls):
    ''' Builds the API from the function signatures from api_define class.
    '''
    typehints = typing.get_type_hints(cls)
    vals = {}
    namespace_prefix = cls.prefix
    for k, v in typehints.items():
        if k == 'prefix':
            continue
        if is_dataclass(v):
            vals[k] = _BuildApi(v)
        else:
            vals[k] = _DmlibFunc(namespace_prefix + k, v)

    return cls(**vals)
