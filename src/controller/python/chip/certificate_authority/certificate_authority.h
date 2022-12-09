#pragma once

#include <lib/core/DataModelTypes.h>
#include <lib/core/NodeId.h>

#define NativeName(FunctionName) chip_python_PythonCertificateIssuerAdapter_##FunctionName
#define NativeFunction(FunctionName, ReturnType, ...)                                                                              \
    extern "C" chip_python_PythonCertificateIssuerAdapter_##FunctionName(__VA_ARGS__)
#define NativeFunctionType(FunctionName, ReturnType, ...)                                                                          \
    ReturnType (*chip_python_PythonCertificateIssuerAdapter_##FunctionName)(__VA_ARGS__)
#define NativeStruct(StructName) struct chip_python_PythonCertificateIssuerAdapter_##StructName

extern "C" {

typedef void PyObject;

typedef NativeFunctionType(SetNodeIdForNextNOCRequestRequestFunc, void, PyObject *, chip::NodeId);
typedef NativeFunctionType(SetFabricIdForNextNOCRequestRequestFunc, void, PyObject *, chip::FabricId);

NativeStruct(GenerateNocChainAfterValidationInputArguments)
{
    chip::NodeId nodeId;
    chip::FabricId fabricId;
    uint32_t * catValues;
    uint32_t catValuesLength;
    uint8_t * pubKey;
    uint32_t pubkeyLen;
};

NativeStruct(GenerateNocChainInputArguments)
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
};

typedef NativeFunctionType(GenerateNOCChainAfterValidationFunc, bool, PyObject * pyContext,
                           const NativeName(GenerateNocChainAfterValidationInputArguments) * inputArgs, uint8_t * rcac,
                           uint32_t * rcacLen, uint8_t * icac, uint32_t * icacLen, uint8_t * noc, uint32_t * nocLen);
typedef NativeFunctionType(IssueNOCChainCallback, void, void * context, bool success, const uint8_t * noc, size_t nocLen,
                           const uint8_t * icac, size_t icacLen, const uint8_t * rcac, size_t rcacLen, const uint8_t * ipk,
                           size_t ipkLen, chip::NodeId adminSubject);
typedef NativeFunctionType(GenerateNocChainFunc, bool, PyObject * pyContext, void * context,
                           const NativeName(GenerateNocChainInputArguments) * inputArgs,
                           NativeName(IssueNOCChainCallback) onFinished);
NativeFunction(OnIssueNOCChainCallback, void)(void * context, bool success, const uint8_t * noc, size_t nocLen,
                                              const uint8_t * icac, size_t icacLen, const uint8_t * rcac, size_t rcacLen,
                                              const uint8_t * ipk, size_t ipkLen, chip::NodeId adminSubject);
NativeFunction(New, void *, PyObject *);
NativeFunction(Free, void, void *);
NativeFunction(SetSetNodeIdForNextNOCRequestFunc, void, void *, NativeFunctionType(SetNodeIdForNextNOCRequestRequestFunc));
NativeFunction(SetSetFabricIdForNextNOCRequestFunc, void, void *, NativeFunctionType(SetFabricIdForNextNOCRequestRequestFunc));
NativeFunction(SetGenerateNOCChainAfterValidationFunc, void, void *, NativeFunctionType(GenerateNOCChainAfterValidationFunc));
NativeFunction(SetGenerateNocChainFunc, void, void *, NativeFunctionType(GenerateNocChainFunc));
}

#undef NativeName(FunctionName) chip_python_PythonCertificateIssuerAdapter_##FunctionName
#undef NativeFunction(FunctionName, ReturnType,                                                                                    \
                      ...) extern "C" chip_python_PythonCertificateIssuerAdapter_##FunctionName(__VA_ARGS__)
#undef NativeFunctionType(FunctionName, ReturnType, ...)                                                                           \
    ReturnType (*chip_python_PythonCertificateIssuerAdapter_##FunctionName)(__VA_ARGS__)
#undef NativeStruct(StructName) struct chip_python_PythonCertificateIssuerAdapter_##StructName
