#pragma once

#include <lib/core/DataModelTypes.h>
#include <lib/core/NodeId.h>

#include <controller/python/ChipDeviceController-StorageDelegate.h>
#include <controller/python/chip/certificate_authority/certificate_authority.h>
#include <controller/python/chip/native/PyChipError.h>

#define NativeName(FunctionName) chip_python_PythonExampleOpCredsIssuer_##FunctionName
#define NativeFunction(FunctionName, ReturnType, ...)                                                                              \
    extern "C" ReturnType chip_python_PythonExampleOpCredsIssuer_##FunctionName(__VA_ARGS__)
#define NativeFunctionType(FunctionName, ReturnType, ...)                                                                          \
    ReturnType (*chip_python_PythonExampleOpCredsIssuer_##FunctionName)(__VA_ARGS__)
#define NativeStruct(StructName) struct chip_python_PythonExampleOpCredsIssuer_##StructName

extern "C" {

typedef void PyObject;

NativeFunction(New, void *, uint32_t);
NativeFunction(Delete, void *);
NativeFunction(Initialize, PyChipError, void *, chip::Controller::Python::StorageAdapter * storageAdapter);
NativeFunction(SetNodeIdForNextNOCRequest, void, void *, chip::NodeId);
NativeFunction(SetFabricIdForNextNOCRequest, void, void *, chip::FabricId);
NativeFunction(GenerateNOCChainAfterValidation, PyChipError, void *,
               const chip_python_PythonCertificateIssuerAdapter_GenerateNocChainAfterValidationInputArguments *, uint8_t * rcac,
               uint32_t * rcacLen, uint8_t * icac, uint32_t * icacLen, uint8_t * noc, uint32_t * nocLen);
NativeFunction(GenerateNOCChain, void, void *, void *,
               const chip_python_PythonCertificateIssuerAdapter_GenerateNocChainInputArguments *,
               chip_python_PythonCertificateIssuerAdapter_IssueNOCChainCallback);
}

#undef NativeName(FunctionName)
#undef NativeFunction(FunctionName, ReturnType, ...)
#undef NativeFunctionType(FunctionName, ReturnType, ...)
#undef NativeStruct(StructName)
