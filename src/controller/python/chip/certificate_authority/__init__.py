from ctypes import *
from typing import *
import dataclasses

import chip.native


@dataclasses.dataclass
class GenerateNocChainAfterValidationInputArgumentsData:
    nodeId: int
    fabricId: int
    catValues: List[int]
    pubKey: bytes


class GenerateNocChainAfterValidationInputArguments(Structure):
    ''' NativeStruct(GenerateNocChainAfterValidationInputArguments)
    {
        chip::NodeId nodeId;
        chip::FabricId fabricId;
        uint32_t * catValues;
        uint8_t * pubKey;
        uint32_t catValuesLength;
        uint32_t pubkeyLen;
    }
    '''

    _fields_ = [
        ('nodeId', c_uint64),
        ('fabricId', c_uint64),
        ('catValues', POINTER(c_uint32)),
        ('catValuesLength', c_uint32),
        ('pubKey', c_char_p),
        ('pubkeyLen', c_uint32)
    ]


@dataclasses.dataclass
class GenerateNocChainAfterValidationOutputArgumentsData:
    rcac: bytes
    icac: bytes
    noc: bytes


@dataclasses.dataclass
class GenerateNocChainInputArgumentsData:
    csrElements: bytes = b''
    csrNonce: bytes = b''
    attensationSignature: bytes = b''
    attestationChallenge: bytes = b''
    dac: bytes = b''
    pai: bytes = b''


class GenerateNocChainInputArguments(ctypes.Structure):
    ''' NativeStruct(GenerateNocChainInputArguments)
    {
        uint8_t * csrElements;
        uint32_t csrElementsLen;
        uint8_t * csrNonce;
        uint32_t csrNonceLen;
        uint8_t * attestationSignature;
        uint32_t attestationSignatureLen;
        uint8_t * attestationChallenge;
        uint32_t attestationChallengeLen;
        uint8_t * DAC;
        uint32_t DACLen;
        uint8_t * PAI;
        uint32_t PAILen;
    }
    '''

    _fields_ = [
        ('csrElements', c_char_p),
        ('csrElementsLen', c_uint32),
        ('csrNonce', c_char_p),
        ('csrNonceLen', c_uint32),
        ('attestationSignature', c_char_p),
        ('attestationSignatureLen', c_uint32),
        ('attestationChallenge', c_char_p),
        ('attestationChallengeLen', c_uint32),
        ('DAC', c_char_p),
        ('DACLen', c_uint32),
        ('PAI', c_char_p),
        ('PAILen', c_uint32),
    ]


SetNodeIdForNextNOCRequestRequestFunc = CFUNCTYPE[None, py_object, c_uint64]
SetFabricIdForNextNOCRequestRequestFunc = CFUNCTYPE[None, py_object, c_uint64]
GenerateNOCChainAfterValidationFunc = CFUNCTYPE[bool, py_object, POINTER(
    GenerateNocChainAfterValidationInputArguments), c_char_p, POINTER(c_uint32), c_char_p, POINTER(c_uint32), c_char_p, POINTER(c_uint32)]
IssueNOCChainCallback = CFUNCTYPE[None, c_void_p, bool, c_char_p,
                                  c_size_t, c_char_p, c_size_t, c_char_p, c_size_t, c_char_p, c_size_t, c_uint64]
GenerateNocChainFunc = CFUNCTYPE[bool, py_object, c_void_p, POINTER(GenerateNocChainInputArguments), IssueNOCChainCallback]


class CertificateAuthorityAdapter:
    def __init__(self, chipStack):
        self._handle = chip.native.GetLibraryHandle()
        self._obj = self._handle.chip_python_PythonCertificateIssuerAdapter_New(py_object(self))
        self._handle.chip_python_PythonCertificateIssuerAdapter_SetSetNodeIdForNextNOCRequestFunc(
            self._obj, _setNodeIdForNextNOCRequestRequestFuncHandler)
        self._handle.chip_python_PythonCertificateIssuerAdapter_SetSetFabricIdForNextNOCRequestFunc(
            self._obj, _setFabricIdForNextNOCRequestRequestFuncHandler)
        self._handle.chip_python_PythonCertificateIssuerAdapter_SetGenerateNOCChainAfterValidationFunc(
            self._obj, _generateNocChainAfterValidationFuncHandler)
        self._handle.chip_python_PythonCertificateIssuerAdapter_SetGenerateNocChainFunc(
            self._obj, _generateNocChainFuncHandler)

        self._chipStack = chipStack
        self.nodeIdForNextNocRequest = 0
        self.fabricIdForNextNocRequest = 0

    def delete(self):
        self._handle.chip_python_PythonCertificateIssuerAdapter_Delete(self._obj)
        self._obj = None

    @property
    def adapter(self):
        return self._obj

    def SetNodeIdForNextNOCRequest(self, nodeId: int):
        self.nodeIdForNextNocRequest = nodeId

    def SetFabricIdForNextNOCRequest(self, fabricId: int):
        self.fabricIdForNextNocRequest = fabricId

    def GenerateNOCChainAfterValidation(self, arg: GenerateNocChainAfterValidationInputArgumentsData) -> GenerateNocChainAfterValidationOutputArgumentsData:
        raise NotImplementedError('GenerateNOCChainAfterValidationFunc should be implemented')

    def GenerateNocChain(self, arg: GenerateNocChainInputArguments, onComplete: Callable[[bool, bytes, bytes, bytes, bytes, int], None]):
        raise NotImplementedError('GenerateNocChainFunc should be implemented')


class ExampleCertificateAuthority(CertificateAuthorityAdapter):
    def __init__(self, chipStack, fabricCredentialsIndex: int, storageAdapter: c_void_p):
        super().__init__(chipStack)

        self._handle = chip.native.GetLibraryHandle()
        self._obj = self._handle.chip_python_PythonExampleOpCredsIssuer_New(fabricCredentialsIndex)
        self._handle.chip_python_PythonExampleOpCredsIssuer_Initialize(self._obj, storageAdapter)

    def delete(self):
        self._handle.chip_python_PythonExampleOpCredsIssuer_Delete(self._obj)
        self._obj = None

    def GenerateNOCChainAfterValidation(self, arg: GenerateNocChainAfterValidationInputArgumentsData) -> GenerateNocChainAfterValidationOutputArgumentsData:
        rcac = create_string_buffer(512)
        rcacLen = c_uint32(512)
        icac = create_string_buffer(512)
        icacLen = c_uint32(512)
        noc = create_string_buffer(512)
        nocLen = c_uint32(512)
        catValuesType = c_uint32 * len(arg.catValues)
        catValues = catValuesType(*arg.catValues)
        pubKey = create_string_buffer(arg.pubKey)
        argIn = GenerateNocChainAfterValidationInputArguments(
            nodeId=arg.nodeId,
            fabricId=arg.fabricId,
            catValues=catValues,
            catValuesLength=len(arg.catValues),
            pubKey=pubKey,
            pubKeyLen=len(arg.pubKey)
        )

        err = self._handle.chip_python_PythonExampleOpCredsIssuer_GenerateNOCChainAfterValidation(
            self._obj,
            pointer(argIn),
            rcac, pointer(rcacLen), icac, pointer(icacLen), noc, pointer(nocLen))

        err.raise_on_error()

        return GenerateNocChainAfterValidationOutputArgumentsData(
            rcac=rcac.raw[:rcacLen.value],
            icac=icac.raw[:icacLen.value],
            noc=noc.raw[nocLen.value]
        )

    def GenerateNocChain(self, arg: GenerateNocChainInputArgumentsData, onComplete: Callable[[bool, bytes, bytes, bytes, bytes, int], None]):
        @IssueNOCChainCallback
        def onCompleteCallback(ctx: c_void_p, success: bool, noc: c_char_p, nocLen: c_size_t, icac: c_char_p, icacLen: c_size_t, rcac: c_char_p, rcacLen: c_size_t, ipk: c_char_p, ipkLen: c_size_t, adminSubject: int):
            onComplete(success, string_at(noc, nocLen)[:], string_at(icac, icacLen)[:],
                       string_at(rcac, rcacLen)[:], string_at(ipk, ipkLen), adminSubject)
            pythonapi.Py_DecRef(onCompleteCallback)

        args = GenerateNocChainInputArguments(
            csrElements=create_string_buffer(arg.csrElements),
            csrElementsLen=len(arg.csrElements),
            csrNonce=create_string_buffer(arg.csrNonce),
            csrNonceLen=len(arg.csrNonce),
            attestationSignature=create_string_buffer(arg.attensationSignature),
            attestationSignatureLen=len(arg.attensationSignature),
            attestationChallenge=create_string_buffer(arg.attestationChallenge),
            attestationChallengeLen=len(arg.attestationChallenge),
            DAC=create_string_buffer(arg.dac),
            DACLen=len(arg.dac),
            PAI=create_string_buffer(arg.pai),
            PAILen=len(arg.pai),
        )

        pythonapi.Py_IncRef(onCompleteCallback)
        if self.nodeIdForNextNocRequest:
            self._handle.SetNodeIdForNextNOCRequest(self._obj, self.nodeIdForNextNocRequest)
        if self.fabricIdForNextNocRequest:
            self._handle.SetFabricIdForNextNOCRequest(self._obj, self.fabricIdForNextNocRequest)
        self._handle.chip_python_PythonExampleOpCredsIssuer_GenerateNocChainFunc(self._obj, None, pointer(args), onCompleteCallback)
        self.nodeIdForNextNocRequest = None
        self.fabricIdForNextNocRequest = None


@GenerateNOCChainAfterValidationFunc
def _generateNocChainAfterValidationFuncHandler(self_: CertificateAuthority, argIn: POINTER(GenerateNocChainAfterValidationInputArguments), rcac: c_char_p, rcacLen: POINTER(c_uint32), icac: c_char_p, icacLen: POINTER(c_uint32), noc: c_char_p, nocLen: POINTER(c_uint32)):
    try:
        data = GenerateNocChainAfterValidationInputArgumentsData()
        argInContent = argIn.contents
        data.nodeId = argInContent.nodeId
        data.fabricId = argInContent.fabricId
        data.catValues = [int(argInContent.catValues[i]) for i in range(int(argInContent.catValuesLength))]
        data.pubKey = string_at(argInContent.pubKey, argInContent.pubLeyLen)[:]

        result = self_.GenerateNOCChainAfterValidation(data)

        memmove(rcac, result.rcac, len(result.rcac))
        memmove(icac, result.icac, len(result.icac))
        memmove(noc, result.noc, len(result.noc))
        rcacLen.contents = len(result.rcac)
        icacLen.contents = len(result.icac)
        nocLen.contents = len(result.noc)

        return True
    except:
        return False


@GenerateNocChainFunc
def _generateNocChainFuncHandler(self_: CertificateAuthority, ctx: c_void_p, argIn: POINTER(GenerateNocChainAfterValidationInputArguments), onComplete: IssueNOCChainCallback):
    try:
        data = GenerateNocChainInputArgumentsData()
        argInContent = argIn.contents

        if argInContent.csrElementsLen:
            data.csrElements = string_at(argInContent.csrElements, argInContent.csrElementsLen)[:]
        if argInContent.csrNonceLen:
            data.csrNonce = string_at(argInContent.csrNonce, argInContent.csrNonceLen)[:]
        if argInContent.attestationSignatureLen:
            data.attensationSignature = string_at(argInContent.attestationSignature, argInContent.attestationSignatureLen)[:]
        if argInContent.attestationChallengeLen:
            data.attestationChallenge = string_at(argInContent.attestationChallenge, argInContent.attestationChallengeLen)[:]
        if argInContent.DACLen:
            data.dac = string_at(argInContent.DAC, argInContent.DACLen)[:]
        if argInContent.csrElementsLen:
            data.pai = string_at(argInContent.PAI, argInContent.PAILen)[:]

        def onCompleteCallback(success: bool, noc: bytes, icac: bytes, rcac: bytes, ipk: bytes, adminSubject: int):
            onComplete(ctx, success, noc, len(noc), icac, len(icac), rcac, len(rcac), ipk, len(ipk), adminSubject)

        self_.GenerateNocChain(data, onCompleteCallback)

        return True
    except:
        return False


@SetNodeIdForNextNOCRequestRequestFunc
def _setNodeIdForNextNOCRequestRequestFuncHandler(self_: CertificateAuthority, nodeId: int):
    self_.SetNodeIdForNextNOCRequest(nodeId)


@SetFabricIdForNextNOCRequestRequestFunc
def _setFabricIdForNextNOCRequestRequestFuncHandler(self_: CertificateAuthority, fabricId: int):
    self_.SetFabricIdForNextNOCRequest(fabricId)


def init():
    handle = chip.native.GetLibraryHandle()
    setter = chip.native.NativeLibraryHandleMethodArguments(
        handle, 'chip_python_PythonCertificateIssuerAdapter_')
    setter.Set('New', c_void_p, py_object)
    setter.Set('Delete', None, c_void_p)
    setter.Set('SetSetNodeIdForNextNOCRequestFunc', None, c_void_p, SetNodeIdForNextNOCRequestRequestFunc)
    setter.Set('SetSetFabricIdForNextNOCRequestFunc', None, c_void_p, SetFabricIdForNextNOCRequestRequestFunc)
    setter.Set('SetGenerateNOCChainAfterValidationFunc', None, c_void_p, GenerateNOCChainAfterValidationFunc)
    setter.Set('SetGenerateNocChainFunc', None, c_void_p, GenerateNocChainFunc)

    setter = chip.native.NativeLibraryHandleMethodArguments(
        handle, 'chip_python_PythonExampleOpCredsIssuer_')
    setter.Set('New', c_void_p, c_uint32)
    setter.Set('Delete', c_void_p,)
    setter.Set('Initialize', chip.native.PyChipError, c_void_p, c_void_p)
    setter.Set('SetNodeIdForNextNOCRequest', None, c_void_p, c_uint64)
    setter.Set('SetFabricIdForNextNOCRequest', None, c_void_p, c_uint64)
    setter.Set('GenerateNOCChainAfterValidation', chip.native.PyChipError, c_void_p, POINTER(
        GenerateNocChainAfterValidationInputArguments), c_char_p, POINTER(c_uint32), c_char_p, POINTER(c_uint32), c_char_p, POINTER(c_uint32))
    setter.Set('GenerateNOCChain', None, c_void_p, c_void_p, POINTER(GenerateNocChainInputArguments), IssueNOCChainCallback)
