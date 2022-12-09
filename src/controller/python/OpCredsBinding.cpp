/*
 *
 *    Copyright (c) 2020-2022 Project CHIP Authors
 *    Copyright (c) 2019-2020 Google LLC.
 *    Copyright (c) 2013-2018 Nest Labs, Inc.
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

#include <algorithm>
#include <type_traits>

#include <controller/python/chip/certificate_authority/certificate_authority.h>

#include "ChipDeviceController-ScriptDevicePairingDelegate.h"
#include "ChipDeviceController-StorageDelegate.h"

#include "controller/python/chip/interaction_model/Delegate.h"

#include <controller/CHIPDeviceController.h>
#include <controller/CHIPDeviceControllerFactory.h>
#include <controller/ExampleOperationalCredentialsIssuer.h>
#include <lib/support/BytesToHex.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/DLLUtil.h>
#include <lib/support/ScopedBuffer.h>
#include <lib/support/TestGroupData.h>
#include <lib/support/logging/CHIPLogging.h>

#include <controller/python/chip/native/PyChipError.h>
#include <credentials/GroupDataProviderImpl.h>
#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>
#include <credentials/attestation_verifier/FileAttestationTrustStore.h>

using namespace chip;
using namespace chip::Controller::Python;

using Py_GenerateNOCChainFunc         = void (*)(void * pyContext, const char * csrElements, const char * attestationSignature,
                                         const char * dac, const char * pai, const char * paa,
                                         Controller::OnNOCChainGeneration onNocChainGenerationFunc);
using Py_SetNodeIdForNextNOCRequest   = void (*)(void * pyContext, NodeId nodeId);
using Py_SetFabricIdForNextNOCRequest = void (*)(void * pyContext, FabricId fabricId);

namespace {
const chip::Credentials::AttestationTrustStore * GetTestFileAttestationTrustStore(const char * paaTrustStorePath)
{
    static chip::Credentials::FileAttestationTrustStore attestationTrustStore{ paaTrustStorePath };

    return &attestationTrustStore;
}
} // namespace

namespace chip {
namespace Controller {
namespace Python {

class PythonCertificateIssuerAdapter : public OperationalCredentialsDelegate
{
public:
    using SetNodeIdRequestFunc           = chip_python_PythonCertificateIssuerAdapter_SetNodeIdForNextNOCRequestRequestFunc;
    using SetFabricIdRequestFunc         = chip_python_PythonCertificateIssuerAdapter_SetFabricIdForNextNOCRequestRequestFunc;
    using GenerateNocChainInputArguments = chip_python_PythonCertificateIssuerAdapter_GenerateNocChainInputArguments;
    using IssueNOCChainCallback          = chip_python_PythonCertificateIssuerAdapter_IssueNOCChainCallback;
    using GenerateNocChainFunc           = chip_python_PythonCertificateIssuerAdapter_GenerateNocChainFunc;
    using GenerateNocChainAfterValidationInputArguments =
        chip_python_PythonCertificateIssuerAdapter_GenerateNocChainAfterValidationInputArguments;
    using GenerateNocChainAfterValidationOutputArguments =
        chip_python_PythonCertificateIssuerAdapter_GenerateNocChainAfterValidationInputArguments;
    using GenerateNOCChainAfterValidationFunc = chip_python_PythonCertificateIssuerAdapter_GenerateNOCChainAfterValidationFunc;

    PythonCertificateIssuerAdapter(PyObject * aPyObject) : mPyContext(aPyObject){};

    void SetSetNodeIdForNextNOCRequestFunc(SetNodeIdForNextNOCRequestRequestFunc aFunc) { mSetNodeIdRequestFunc = aFunc; }

    void SetSetFabricIdForNextNOCRequestFunc(SetFabricIdForNextNOCRequestRequestFunc aFunc) { mSetFabricIdRequestFunc = aFunc; }

    void SetGenerateNOCChainAfterValidationFunc(GenerateNOCChainAfterValidationFunc aFunc)
    {
        mGenerateNOCChainAfterValidationFunc = aFunc;
    }

    void SetGenerateNocChainFunc(GenerateNocChainFunc aFunc) { mGenerateNocChainFunc = aFunc; }

    void SetNodeIdForNextNOCRequest(NodeId nodeId) override { mSetNodeIdRequestFunc(mPyContext, nodeId); }

    void SetFabricIdForNextNOCRequest(FabricId fabricId) override { mSetFabricIdRequestFunc(mPyContext, fabricId); }

    CHIP_ERROR GenerateNOCChain(NodeId nodeId, FabricId fabricId, const CATValues & cats, const Crypto::P256PublicKey & pubKey,
                                MutableByteSpan & rcac, MutableByteSpan & icac, MutableByteSpan & noc)
    {
        GenerateNocChainAfterValidationInputArguments req = GenerateNocChainAfterValidationInputArguments{
            .nodeId          = nodeId,
            .fabricId        = fabricId,
            .catValues       = cats.values.data(),
            .catValuesLength = cats.size(),
            .pubKey          = pubKey.Bytes(),
            .pubkeyLen       = pubKey.Length(),
        };

        uint8_t rcacData = rcac.data();
        uint32_t rcacLen = rcac.size();
        uint8_t icacData = icac.data();
        uint32_t icacLen = icac.size();
        uint8_t nocData  = noc.data();
        uint32_t nocLen  = noc.size();

        VerifyOrReturnError(mGenerateNOCChainAfterValidationFunc(&req, &rcacData, &rcacLen, &icacData, &icacLen, &nocData, &nocLen),
                            CHIP_ERROR_INTERNAL);

        rcac = MutableByteSpan(rcacData, rcacLen);
        icac = MutableByteSpan(icacData, icacLen);
        noc  = MutableByteSpan(nocData, nocLen);

        return CHIP_NO_ERROR;
    }

    CHIP_ERROR
    GenerateNOCChain(const ByteSpan & csrElements, const ByteSpan & csrNonce, const ByteSpan & attestationSignature,
                     const ByteSpan & attestationChallenge, const ByteSpan & DAC, const ByteSpan & PAI,
                     Callback::Callback<OnNOCChainGeneration> * onCompletion) override
    {
        GenerateNocChainInputArguments req = GenerateNocChainInputArguments{
            .csrElements             = csrElements.data(),
            .csrElementsLen          = csrElements.size(),
            .csrNonce                = csrNonce.data(),
            .csrNonceLen             = csrElements.size(),
            .attestationSignature    = attestationSignature.data(),
            .attestationSignatureLen = attestationSignature.size(),
            .attestationChallenge    = attestationChallenge.data(),
            .attestationChallengeLen = attestationChallenge.size(),
            .DAC                     = DAC.data(),
            .DACLen                  = DAC.size(),
            .PAI                     = PAI.data(),
            .PAILen                  = PAI.size(),
        };
        mGenerateNocChainFunc(onCompletion, &req, OnIssueNOCChainCallback);
        return CHIP_NO_ERROR;
    }

private:
    SetNodeIdRequestFunc mSetNodeIdRequestFunc;
    SetFabricIdRequestFunc mSetFabricIdRequestFunc;

    GenerateNOCChainAfterValidationFunc mGenerateNOCChainAfterValidationFunc;
    GenerateNocChainFunc mGenerateNocChainFunc;

    static void OnIssueNOCChainCallback(void * context, bool success, const uint8_t * noc, size_t nocLen, const uint8_t * icac,
                                        size_t icacLen, const uint8_t * rcac, size_t rcacLen, const uint8_t * ipk, size_t ipkLen,
                                        NodeId adminSubject)
    {
        Callback::Callback<OnNOCChainGeneration> * cb = static_cast<Callback::Callback<OnNOCChainGeneration> *>(context);
        onCompletion->mCall(onCompletion->mContext, success ? CHIP_NO_ERROR : CHIP_ERROR_INTERNAL, ByteSpan(noc, nocLen),
                            ByteSpan(icac, icacLen), ByteSpan(rcac, rcacLen),
                            ipkLen != 0 ? MakeOptional(ByteSpan(ipk, ipkLen)) : NullOptional,
                            adminSubject != 0 ? MakeOptional(adminSubject) : NullOptional);
    }

    PyObject * mPyContext;
};

class PythonExampleOpCredsIssuer
{
public:
    PythonExampleOpCredsIssuer(uint32_t fabricCredentialsIndex) :
        mOnNOCChainGenerationBridge(OnNOCChainGenerationBridgeFunc, this), mExampleOpCredsIssuer(fabricCredentialsIndex)
    {}

    CHIP_ERROR Initialize(PersistentStorageDelegate & storageDelegate) { return mExampleOpCredsIssuer.Initialize(storageDelegate); }

    void SetNodeIdForNextNOCRequest(NodeId nodeId) override { mExampleOpCredsIssuer.SetNodeIdForNextNOCRequest(nodeId); }

    void SetFabricIdForNextNOCRequest(FabricId fabricId) override { mExampleOpCredsIssuer.SetFabricIdForNextNOCRequest(fabricId); }

    CHIP_ERROR
    GenerateNOCChainAfterValidation(const PythonCertificateIssuerAdapter::GenerateNocChainAfterValidationInputArguments * inputArgs,
                                    uint8_t * rcac, uint32_t * rcacLen, uint8_t * icac, uint32_t * icacLen, uint8_t * noc,
                                    uint32_t * nocLen)
    {
        CATValues & cats;
        Crypto::P256PublicKey & pubKey;

        MutableByteSpan rcac(*rcac, *rcacLen);
        MutableByteSpan icac(*icac, *icacLen);
        MutableByteSpan noc(*noc, *nocLen);

        std::fill(cats.values.begin(), cats.values.end(), kUndefinedCAT);
        std::copy(inputArgs->catValues, inputArgs->catValues + inputArgs->catValuesLength, cats.values.begin());

        VerifyOrReturn(kP256_PublicKey_Length == inputArgs->pubkeyLen, CHIP_ERROR_INVALID_ARGS);
        pubKey = FixedByteSpan<kP256_PublicKey_Length>(inputArgs->pubKey);

        ReturnErrorOnFailure(mExampleOpCredsIssuer.GenerateNOCChainAfterValidation(inputArgs->nodeId, inputArgs->fabricId, cats,
                                                                                   pubKey, rcac, icac, noc));
        *rcacLen = rcac.size();
        *icacLen = icac.size();
        *nocLen  = noc.size();

        return CHIP_NO_ERROR;
    }

    CHIP_ERROR GenerateNOCChain(void * context, const PythonCertificateIssuerAdapter::GenerateNocChainInputArguments * inputArgs,
                                PythonCertificateIssuerAdapter::IssueNOCChainCallback onFinished)
    {
        VerifyOrReturn(mPendingGenerateNocContext == nullptr && mPendingGenerateNocOnFinished == nullptr, CHIP_ERROR_BUSY);
        mPendingGenerateNocContext    = context;
        mPendingGenerateNocOnFinished = onFinished;

        CHIP_ERROR err = mExampleOpCredsIssuer.GenerateNOCChain(
            ByteSpan(inputArgs->csrElements, inputArgs->csrElementsLen), ByteSpan(inputArgs->csrNonce, inputArgs->csrNonceLen),
            ByteSpan(inputArgs->attestationSignature, inputArgs->attestationSignatureLen),
            ByteSpan(inputArgs->attestationChallenge, inputArgs->attestationChallengeLen),
            ByteSpan(inputArgs->DAC, inputArgs->DACLen), ByteSpan(inputArgs->PAI, inputArgs->PAILen), &mOnNOCChainGenerationBridge);

        if (err != CHIP_NO_ERROR)
        {
            mPendingGenerateNocContext    = nullptr;
            mPendingGenerateNocOnFinished = nullptr;
        }

        return err;
    }

    static void OnNOCChainGenerationBridgeFunc(void * context, CHIP_ERROR status, const ByteSpan & noc, const ByteSpan & icac,
                                               const ByteSpan & rcac, Optional<Crypto::AesCcm128KeySpan> ipk,
                                               Optional<NodeId> adminSubject)
    {
        PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(context);
        this_->mPendingGenerateNocOnFinished(this_->mPendingGenerateNocContext, status == CHIP_NO_ERROR, noc.data(), noc.size(),
                                             icac.data(), icac.size(), rcac.data(), rcac.size(),
                                             ipk.HasValue() ? ipc.Value().data() : nullptr, ipk.HasValue() ? ipc.Value().size() : 0,
                                             adminSubject.ValueOr(kUndefinedNodeId));
        this_->mPendingGenerateNocContext    = nullptr;
        this_->mPendingGenerateNocOnFinished = nullptr;
    }

private:
    void * mPendingGenerateNocContext                                                   = nullptr;
    PythonCertificateIssuerAdapter::IssueNOCChainCallback mPendingGenerateNocOnFinished = nullptr;

    Callback::Callback<OnNOCChainGeneration> mOnNOCChainGenerationBridge;
    ExampleOperationalCredentialsIssuer mExampleOpCredsIssuer;
};

class OperationalCredentialsAdapter : public OperationalCredentialsDelegate
{
public:
    OperationalCredentialsAdapter(uint32_t fabricCredentialsIndex) : mExampleOpCredsIssuer(fabricCredentialsIndex) {}

    CHIP_ERROR Initialize(PersistentStorageDelegate & storageDelegate) { return mExampleOpCredsIssuer.Initialize(storageDelegate); }

    CHIP_ERROR GenerateNOCChain(NodeId nodeId, FabricId fabricId, const CATValues & cats, const Crypto::P256PublicKey & pubKey,
                                MutableByteSpan & rcac, MutableByteSpan & icac, MutableByteSpan & noc)
    {
        return mExampleOpCredsIssuer.GenerateNOCChainAfterValidation(nodeId, fabricId, cats, pubKey, rcac, icac, noc);
    }

    void SetMaximallyLargeCertsUsed(bool enabled) { mExampleOpCredsIssuer.SetMaximallyLargeCertsUsed(enabled); }

private:
    CHIP_ERROR GenerateNOCChain(const ByteSpan & csrElements, const ByteSpan & csrNonce, const ByteSpan & attestationSignature,
                                const ByteSpan & attestationChallenge, const ByteSpan & DAC, const ByteSpan & PAI,
                                Callback::Callback<OnNOCChainGeneration> * onCompletion) override
    {
        return mExampleOpCredsIssuer.GenerateNOCChain(csrElements, csrNonce, attestationSignature, attestationChallenge, DAC, PAI,
                                                      onCompletion);
    }

    void SetNodeIdForNextNOCRequest(NodeId nodeId) override { mExampleOpCredsIssuer.SetNodeIdForNextNOCRequest(nodeId); }

    void SetFabricIdForNextNOCRequest(FabricId fabricId) override { mExampleOpCredsIssuer.SetFabricIdForNextNOCRequest(fabricId); }

    ExampleOperationalCredentialsIssuer mExampleOpCredsIssuer;
};

extern "C" {
void * chip_python_PythonCertificateIssuerAdapter_New(PyObject * aObject)
{
    PythonCertificateIssuerAdapter * this_ = new PythonCertificateIssuerAdapter(aObject);
    return this_;
}

void chip_python_PythonCertificateIssuerAdapter_Free(void * aContext)
{
    PythonCertificateIssuerAdapter * this_ = static_cast<PythonCertificateIssuerAdapter *>(aContext);
    delete this_;
}

void chip_python_PythonCertificateIssuerAdapter_SetSetNodeIdForNextNOCRequestFunc(
    void * aContext, chip_python_PythonCertificateIssuerAdapter_SetNodeIdForNextNOCRequestRequestFunc aFunc)
{
    PythonCertificateIssuerAdapter * this_ = static_cast<PythonCertificateIssuerAdapter *>(aContext);
    this_->SetSetNodeIdForNextNOCRequestRequestFunc(aFunc);
}

void chip_python_PythonCertificateIssuerAdapter_SetSetFabricIdForNextNOCRequestFunc(
    void * aContext, chip_python_PythonCertificateIssuerAdapter_SetFabricIdForNextNOCRequestRequestFunc aFunc)
{
    PythonCertificateIssuerAdapter * this_ = static_cast<PythonCertificateIssuerAdapter *>(aContext);
    this_->SetSetFabricIdForNextNOCRequestRequestFunc(aFunc);
}

void chip_python_PythonCertificateIssuerAdapter_SetGenerateNOCChainAfterValidationFunc(
    void * aContext, chip_python_PythonCertificateIssuerAdapter_GenerateNOCChainAfterValidationFunc aFunc)
{
    PythonCertificateIssuerAdapter * this_ = static_cast<PythonCertificateIssuerAdapter *>(aContext);
    this_->SetGenerateNOCChainAfterValidationFunc(aFunc);
}

void chip_python_PythonCertificateIssuerAdapter_SetGenerateNocChainFunc(
    void * aContext, chip_python_PythonCertificateIssuerAdapter_GenerateNocChainFunc aFunc)
{
    PythonCertificateIssuerAdapter * this_ = static_cast<PythonCertificateIssuerAdapter *>(aContext);
    this_->SetGenerateNocChainFunc(aFunc);
}

void * chip_python_PythonExampleOpCredsIssuer_New(void * This, uint32_t fabricCredentialsIndex)
{
    PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(This);
    return new PythonExampleOpCredsIssuer(fabricCredentialsIndex);
}

void chip_python_PythonExampleOpCredsIssuer_Delete(void * This)
{
    PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(This);
    delete this_;
}

PyChipError chip_python_PythonExampleOpCredsIssuer_Initialize(void * This,
                                                              chip::Controller::Python::StorageAdapter * storageAdapter)
{
    PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(This);
    return ToPyChipError(this_->Initialize(*storageAdapter));
}

void chip_python_PythonExampleOpCredsIssuer_SetNodeIdForNextNOCRequest(void * This, chip::NodeId aNodeId)
{
    PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(This);
    this_->SetNodeIdForNextNOCRequest(aNodeId);
}

void chip_python_PythonExampleOpCredsIssuer_SetFabricIdForNextNOCRequest(void * This, chip::FabricId aFabricId)
{
    PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(This);
    this_->SetFabricIdForNextNOCRequest(aFabricId);
}

PyChipError chip_python_PythonExampleOpCredsIssuer_GenerateNOCChainAfterValidation(
    void * This, const chip_python_PythonCertificateIssuerAdapter_GenerateNocChainAfterValidationInputArguments * inputArgs,
    uint8_t * rcac, uint32_t * rcacLen, uint8_t * icac, uint32_t * icacLen, uint8_t * noc, uint32_t * nocLen)
{
    PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(This);
    return ToPyChipError(this_->GenerateNOCChainAfterValidation(inputArgs, rcac, rcacLen, icac, icacLen, noc, nocLen));
}

PyChipError chip_python_PythonExampleOpCredsIssuer_GenerateNOCChain(
    void * This, void * aContext, const chip_python_PythonCertificateIssuerAdapter_GenerateNocChainInputArguments * inputArgs,
    chip_python_PythonCertificateIssuerAdapter_IssueNOCChainCallback onComplete)
{
    PythonExampleOpCredsIssuer * this_ = static_cast<PythonExampleOpCredsIssuer *>(This);
    return ToPyChipError(this_->GenerateNOCChain(aContext, inputArgs, onComplete));
}
}

} // namespace Python
} // namespace Controller
} // namespace chip

extern chip::Credentials::GroupDataProviderImpl sGroupDataProvider;
extern chip::Controller::ScriptDevicePairingDelegate sPairingDelegate;

class TestCommissioner : public chip::Controller::AutoCommissioner
{
public:
    TestCommissioner() { Reset(); }
    ~TestCommissioner() {}
    CHIP_ERROR SetCommissioningParameters(const chip::Controller::CommissioningParameters & params) override
    {
        mIsWifi   = false;
        mIsThread = false;
        if (params.GetWiFiCredentials().HasValue())
        {
            mIsWifi = true;
        }
        else if (params.GetThreadOperationalDataset().HasValue())
        {
            mIsThread = true;
        }
        return chip::Controller::AutoCommissioner::SetCommissioningParameters(params);
    }
    CHIP_ERROR CommissioningStepFinished(CHIP_ERROR err,
                                         chip::Controller::CommissioningDelegate::CommissioningReport report) override
    {
        mTestCommissionerUsed = true;
        if (mFailOnReportAfterStage == report.stageCompleted)
        {
            return CHIP_ERROR_INTERNAL;
        }
        if (mSimulateFailureOnStage == report.stageCompleted)
        {
            // Pretend we received an error from the device during this stage
            err = CHIP_ERROR_INTERNAL;
        }
        return chip::Controller::AutoCommissioner::CommissioningStepFinished(err, report);
    }
    // This will cause the COMMISSIONER to fail after the given stage. Setting this to kSecurePairing will cause the
    // StartCommissioning call to fail.
    bool SimulateFailAfter(chip::Controller::CommissioningStage stage)
    {
        ChipLogProgress(Controller, "setting simulate fail after stage %s", chip::Controller::StageToString(stage));
        if (!ValidStage(stage) && stage != chip::Controller::CommissioningStage::kError)
        {
            return false;
        }
        mSimulateFailureOnStage = stage;
        return true;
    }
    bool SimulateFailOnReport(chip::Controller::CommissioningStage stage)
    {
        if (!ValidStage(stage) && stage != chip::Controller::CommissioningStage::kError)
        {
            return false;
        }
        mFailOnReportAfterStage = stage;
        return true;
    }
    bool CheckCallbacks()
    {
        bool successFailureOk;
        bool updatesOk;
        if (mFailOnReportAfterStage != chip::Controller::CommissioningStage::kError)
        {
            successFailureOk = mReceivedCommissioningFailureStage == mFailOnReportAfterStage && !mReceivedCommissioningSuccess;
            updatesOk        = StatusUpdatesOk(mFailOnReportAfterStage);
        }
        else if (mSimulateFailureOnStage != chip::Controller::CommissioningStage::kError)
        {
            successFailureOk = mReceivedCommissioningFailureStage == mSimulateFailureOnStage && !mReceivedCommissioningSuccess;
            updatesOk        = StatusUpdatesOk(mSimulateFailureOnStage);
        }
        else
        {
            successFailureOk = mReceivedCommissioningSuccess;
            updatesOk        = StatusUpdatesOk(chip::Controller::CommissioningStage::kError);
        }
        ChipLogProgress(Controller, "Checking callbacks: success failure ok? %d updates ok? %d", successFailureOk, updatesOk);
        return successFailureOk && updatesOk;
    }
    bool CheckPaseConnection(NodeId nodeId)
    {
        bool paseShouldBeOpen = false;
        if (chip::to_underlying(mFailOnReportAfterStage) >=
                chip::to_underlying(chip::Controller::CommissioningStage::kWiFiNetworkSetup) ||
            chip::to_underlying(mSimulateFailureOnStage) >=
                chip::to_underlying(chip::Controller::CommissioningStage::kWiFiNetworkSetup))
        {
            // Pase should be open still
            paseShouldBeOpen = true;
        }
        CommissioneeDeviceProxy * proxy;
        bool paseIsOpen =
            (chip::Controller::AutoCommissioner::GetCommissioner()->GetDeviceBeingCommissioned(nodeId, &proxy) == CHIP_NO_ERROR);
        ChipLogProgress(Controller, "Checking pase connection state: Should be open? %d is open? %d", paseShouldBeOpen, paseIsOpen);

        return paseShouldBeOpen == paseIsOpen;
    }
    void Reset()
    {
        mTestCommissionerUsed              = false;
        mReceivedCommissioningSuccess      = false;
        mReceivedCommissioningFailureStage = chip::Controller::CommissioningStage::kError;
        for (size_t i = 0; i < kNumCommissioningStages; ++i)
        {
            mReceivedStageSuccess[i] = false;
            mReceivedStageFailure[i] = false;
        }
        mSimulateFailureOnStage = chip::Controller::CommissioningStage::kError;
        mFailOnReportAfterStage = chip::Controller::CommissioningStage::kError;
    }
    bool GetTestCommissionerUsed() { return mTestCommissionerUsed; }
    void OnCommissioningSuccess(chip::PeerId peerId) { mReceivedCommissioningSuccess = true; }
    void OnCommissioningFailure(chip::PeerId peerId, CHIP_ERROR error, chip::Controller::CommissioningStage stageFailed,
                                chip::Optional<chip::Credentials::AttestationVerificationResult> additionalErrorInfo)
    {
        mReceivedCommissioningFailureStage = stageFailed;
    }
    void OnCommissioningStatusUpdate(chip::PeerId peerId, chip::Controller::CommissioningStage stageCompleted, CHIP_ERROR error)
    {
        if (error == CHIP_NO_ERROR)
        {
            mReceivedStageSuccess[chip::to_underlying(stageCompleted)] = true;
        }
        else
        {
            mReceivedStageFailure[chip::to_underlying(stageCompleted)] = true;
        }
    }

private:
    static constexpr uint8_t kNumCommissioningStages = chip::to_underlying(chip::Controller::CommissioningStage::kCleanup) + 1;
    chip::Controller::CommissioningStage mSimulateFailureOnStage            = chip::Controller::CommissioningStage::kError;
    chip::Controller::CommissioningStage mFailOnReportAfterStage            = chip::Controller::CommissioningStage::kError;
    bool mTestCommissionerUsed                                              = false;
    bool mReceivedCommissioningSuccess                                      = false;
    chip::Controller::CommissioningStage mReceivedCommissioningFailureStage = chip::Controller::CommissioningStage::kError;
    bool mReceivedStageSuccess[kNumCommissioningStages];
    bool mReceivedStageFailure[kNumCommissioningStages];
    bool mIsWifi   = false;
    bool mIsThread = false;
    bool ValidStage(chip::Controller::CommissioningStage stage)
    {
        if (!mIsWifi &&
            (stage == chip::Controller::CommissioningStage::kWiFiNetworkEnable ||
             stage == chip::Controller::CommissioningStage::kWiFiNetworkSetup))
        {
            return false;
        }
        if (!mIsThread &&
            (stage == chip::Controller::CommissioningStage::kThreadNetworkEnable ||
             stage == chip::Controller::CommissioningStage::kThreadNetworkSetup))
        {
            return false;
        }
        if (stage == chip::Controller::CommissioningStage::kError || stage == chip::Controller::CommissioningStage::kSecurePairing)
        {
            return false;
        }
        return true;
    }
    bool StatusUpdatesOk(chip::Controller::CommissioningStage failedStage)
    {
        // Because we're just simulating a failure here, we will have received a success callback even for the failed stage.
        for (uint8_t i = 0; i < kNumCommissioningStages; ++i)
        {
            if (mReceivedStageFailure[i])
            {
                return false;
            }
            if (!ValidStage(static_cast<chip::Controller::CommissioningStage>(i)))
            {
                continue;
            }
            // Anything above our current stage we won't have received a callback for. We also expect that the "failed" stage will
            // have a success callback because we're just faking the failure here and overwriting error.
            if (i == chip::to_underlying(failedStage))
            {
                break;
            }
        }
        return true;
    }
};
TestCommissioner sTestCommissioner;

extern "C" {
struct OpCredsContext
{
    PythonCertificateIssuerAdapter * mAdapter;
    void * mPyContext;
};

void * pychip_OpCreds_InitializeDelegate(void * pyContext, uint32_t fabricCredentialsIndex,
                                         PythonCertificateIssuerAdapter * certificateAdapter)
{
    auto context      = Platform::MakeUnique<OpCredsContext>();
    context->mAdapter = certificateAdapter;

    return context.release();
}

void pychip_OnCommissioningSuccess(PeerId peerId)
{
    sTestCommissioner.OnCommissioningSuccess(peerId);
}
void pychip_OnCommissioningFailure(chip::PeerId peerId, CHIP_ERROR error, chip::Controller::CommissioningStage stageFailed,
                                   chip::Optional<chip::Credentials::AttestationVerificationResult> additionalErrorInfo)
{
    sTestCommissioner.OnCommissioningFailure(peerId, error, stageFailed, additionalErrorInfo);
}
void pychip_OnCommissioningStatusUpdate(chip::PeerId peerId, chip::Controller::CommissioningStage stageCompleted, CHIP_ERROR err)
{
    return sTestCommissioner.OnCommissioningStatusUpdate(peerId, stageCompleted, err);
}

PyChipError pychip_OpCreds_AllocateController(OpCredsContext * context, chip::Controller::DeviceCommissioner ** outDevCtrl,
                                              FabricId fabricId, chip::NodeId nodeId, chip::VendorId adminVendorId,
                                              const char * paaTrustStorePath, bool useTestCommissioner,
                                              bool enableServerInteractions, CASEAuthTag * caseAuthTags, uint32_t caseAuthTagLen)
{
    ChipLogDetail(Controller, "Creating New Device Controller");

    VerifyOrReturnError(context != nullptr, ToPyChipError(CHIP_ERROR_INVALID_ARGUMENT));

    auto devCtrl = std::make_unique<chip::Controller::DeviceCommissioner>();
    VerifyOrReturnError(devCtrl != nullptr, ToPyChipError(CHIP_ERROR_NO_MEMORY));

    if (paaTrustStorePath == nullptr)
    {
        paaTrustStorePath = "./credentials/development/paa-root-certs";
    }

    ChipLogProgress(Support, "Using device attestation PAA trust store path %s.", paaTrustStorePath);

    // Initialize device attestation verifier
    const chip::Credentials::AttestationTrustStore * testingRootStore = GetTestFileAttestationTrustStore(paaTrustStorePath);
    SetDeviceAttestationVerifier(GetDefaultDACVerifier(testingRootStore));

    chip::Crypto::P256Keypair ephemeralKey;
    CHIP_ERROR err = ephemeralKey.Initialize();
    VerifyOrReturnError(err == CHIP_NO_ERROR, ToPyChipError(err));

    chip::Platform::ScopedMemoryBuffer<uint8_t> noc;
    ReturnErrorCodeIf(!noc.Alloc(Controller::kMaxCHIPDERCertLength), ToPyChipError(CHIP_ERROR_NO_MEMORY));
    MutableByteSpan nocSpan(noc.Get(), Controller::kMaxCHIPDERCertLength);

    chip::Platform::ScopedMemoryBuffer<uint8_t> icac;
    ReturnErrorCodeIf(!icac.Alloc(Controller::kMaxCHIPDERCertLength), ToPyChipError(CHIP_ERROR_NO_MEMORY));
    MutableByteSpan icacSpan(icac.Get(), Controller::kMaxCHIPDERCertLength);

    chip::Platform::ScopedMemoryBuffer<uint8_t> rcac;
    ReturnErrorCodeIf(!rcac.Alloc(Controller::kMaxCHIPDERCertLength), ToPyChipError(CHIP_ERROR_NO_MEMORY));
    MutableByteSpan rcacSpan(rcac.Get(), Controller::kMaxCHIPDERCertLength);

    CATValues catValues;

    if (caseAuthTagLen > kMaxSubjectCATAttributeCount)
    {
        ChipLogError(Controller, "Too many of CASE Tags (%u) exceeds kMaxSubjectCATAttributeCount",
                     static_cast<unsigned>(caseAuthTagLen));
        return ToPyChipError(CHIP_ERROR_INVALID_ARGUMENT);
    }

    memcpy(catValues.values.data(), caseAuthTags, caseAuthTagLen * sizeof(CASEAuthTag));

    err = context->mAdapter->GenerateNOCChain(nodeId, fabricId, catValues, ephemeralKey.Pubkey(), rcacSpan, icacSpan, nocSpan);
    VerifyOrReturnError(err == CHIP_NO_ERROR, ToPyChipError(err));

    Controller::SetupParams initParams;
    initParams.pairingDelegate                = &sPairingDelegate;
    initParams.operationalCredentialsDelegate = context->mAdapter.get();
    initParams.operationalKeypair             = &ephemeralKey;
    initParams.controllerRCAC                 = rcacSpan;
    initParams.controllerICAC                 = icacSpan;
    initParams.controllerNOC                  = nocSpan;
    initParams.enableServerInteractions       = enableServerInteractions;
    initParams.controllerVendorId             = adminVendorId;
    initParams.permitMultiControllerFabrics   = true;

    if (useTestCommissioner)
    {
        initParams.defaultCommissioner = &sTestCommissioner;
        sPairingDelegate.SetCommissioningSuccessCallback(pychip_OnCommissioningSuccess);
        sPairingDelegate.SetCommissioningFailureCallback(pychip_OnCommissioningFailure);
        sPairingDelegate.SetCommissioningStatusUpdateCallback(pychip_OnCommissioningStatusUpdate);
    }

    err = Controller::DeviceControllerFactory::GetInstance().SetupCommissioner(initParams, *devCtrl);
    VerifyOrReturnError(err == CHIP_NO_ERROR, ToPyChipError(err));

    // Setup IPK in Group Data Provider for controller after Commissioner init which sets-up the fabric table entry
    uint8_t compressedFabricId[sizeof(uint64_t)] = { 0 };
    chip::MutableByteSpan compressedFabricIdSpan(compressedFabricId);

    err = devCtrl->GetCompressedFabricIdBytes(compressedFabricIdSpan);
    VerifyOrReturnError(err == CHIP_NO_ERROR, ToPyChipError(err));

    ChipLogProgress(Support, "Setting up group data for Fabric Index %u with Compressed Fabric ID:",
                    static_cast<unsigned>(devCtrl->GetFabricIndex()));
    ChipLogByteSpan(Support, compressedFabricIdSpan);

    chip::ByteSpan defaultIpk = chip::GroupTesting::DefaultIpkValue::GetDefaultIpk();
    err =
        chip::Credentials::SetSingleIpkEpochKey(&sGroupDataProvider, devCtrl->GetFabricIndex(), defaultIpk, compressedFabricIdSpan);
    VerifyOrReturnError(err == CHIP_NO_ERROR, ToPyChipError(err));

    *outDevCtrl = devCtrl.release();

    return ToPyChipError(CHIP_NO_ERROR);
}

PyChipError pychip_OpCreds_SetMaximallyLargeCertsUsed(OpCredsContext * context, bool enabled)
{
    VerifyOrReturnError(context != nullptr && context->mAdapter != nullptr, ToPyChipError(CHIP_ERROR_INCORRECT_STATE));

    context->mAdapter->SetMaximallyLargeCertsUsed(enabled);

    return ToPyChipError(CHIP_NO_ERROR);
}

void pychip_OpCreds_FreeDelegate(OpCredsContext * context)
{
    Platform::Delete(context);
}

PyChipError pychip_DeviceController_DeleteDeviceController(chip::Controller::DeviceCommissioner * devCtrl)
{
    if (devCtrl != nullptr)
    {
        devCtrl->Shutdown();
        delete devCtrl;
    }

    return ToPyChipError(CHIP_NO_ERROR);
}

bool pychip_TestCommissionerUsed()
{
    return sTestCommissioner.GetTestCommissionerUsed();
}

bool pychip_TestCommissioningCallbacks()
{
    return sTestCommissioner.CheckCallbacks();
}

bool pychip_TestPaseConnection(NodeId nodeId)
{
    return sTestCommissioner.CheckPaseConnection(nodeId);
}

void pychip_ResetCommissioningTests()
{
    sTestCommissioner.Reset();
}

// Returns True if this is a valid test, false otherwise
bool pychip_SetTestCommissionerSimulateFailureOnStage(uint8_t failStage)
{
    return sTestCommissioner.SimulateFailAfter(static_cast<chip::Controller::CommissioningStage>(failStage));
}
bool pychip_SetTestCommissionerSimulateFailureOnReport(uint8_t failStage)
{
    return sTestCommissioner.SimulateFailOnReport(static_cast<chip::Controller::CommissioningStage>(failStage));
}

} // extern "C"
