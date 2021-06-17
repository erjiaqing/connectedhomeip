from dataclasses import dataclass
import threading
from typing import Any, Callable, Dict, List
from construct import Array
import chip.interaction_model.delegate as imdelegate
import chip.exceptions
import chip

@dataclass
class AttributeRead:
    Path: imdelegate.IMClusterAttributeInfo
    Value: Any
    Version: int = 0

def ReadAttributeSync(nodeid: int, adminid: int, path: List[Dict]):
    responseData = []
    responseError = None
    responseLock = threading.RLock()
    responseCV = threading.Condition(responseLock)

    def OnData(path: imdelegate.IMClusterAttributeInfo, data: Any, status: int):
        nonlocal responseData
        pathString = "{}/{}/{}".format(path.EndpointId,
                                       path.ClusterId, path.AttributeId)
        responseData.append(AttributeRead(Path=path, Value=data['Any'], Version=0))
        print("ReadData: Path: {}  Value: {}".format(pathString, data['Any']))

    def OnEnd(error: int):
        nonlocal responseError
        nonlocal responseLock
        nonlocal responseCV
        with responseLock:
            responseError = error
            responseCV.notify_all()
        print("Read Interaction End with error code {}".format(error))

    readHandle = imdelegate.GetReadHandle()
    imdelegate.RegisterReadCallback(
        readHandle, imdelegate.ReadCallback(OnData, OnEnd))

    structPath = Array(len(path), imdelegate.IMClusterAttributeInfo)
    data = structPath.build(path)
    handle = chip.native.GetLibraryHandle()
    res = handle.pychip_im_SendReadRequest(
        nodeid, adminid, data, len(path), readHandle)
    if res != 0:
        imdelegate.CancelReadCallback(readHandle)
        raise chip.exceptions.ChipStackError(res)

    with responseLock:
        while responseError is None:
            responseCV.wait()

    if responseError != 0:
        raise chip.exceptions.ChipStackError(responseError)

    return responseData
