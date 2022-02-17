#include <ChipDeviceController-ScriptDeviceAddressUpdateDelegate.h>
#include <ChipDeviceController-ScriptDevicePairingDelegate.h>
#include <PersistentStorage.h>
#include <controller/CHIPDeviceControllerFactory.h>
#include <controller/ExampleOperationalCredentialsIssuer.h>
#include <core/CHIPBuildConfig.h>
#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <lib/core/CHIPVendorIdentifiers.hpp>
#include <lib/support/CodeUtils.h>
#include <lib/support/ScopedBuffer.h>
#include <pthread.h>
#include <rpc/matter_controller.h>

#include "rpc.h"

using namespace chip;
using namespace chip::Controller;

namespace {

pthread_t sPlatformMainThread;
Controller::DeviceCommissioner sController;

PersistentStorage sStorageDelegate;
chip::rpc::RPCDevicePairingDelegate sPairingDelegate;
chip::Controller::ScriptDeviceAddressUpdateDelegate sDeviceAddressUpdateDelegate;
chip::Controller::ExampleOperationalCredentialsIssuer sOperationalCredentialsIssuer;
chip::SimpleFabricStorage sFabricStorage;
chip::Platform::ScopedMemoryBuffer<uint8_t> sSsidBuf;
chip::Platform::ScopedMemoryBuffer<uint8_t> sCredsBuf;
chip::Platform::ScopedMemoryBuffer<uint8_t> sThreadBuf;
chip::Controller::CommissioningParameters sCommissioningParameters;

void * PlatformMainLoop(void *)
{
    ChipLogProgress(DeviceLayer, "Platform main loop started.");
    chip::DeviceLayer::PlatformMgr().RunEventLoop();
    ChipLogProgress(DeviceLayer, "Platform main loop completed.");
    return nullptr;
}

CHIP_ERROR InitController()
{
    NodeId localDeviceId = 11223344;

    // Initialize device attestation verifier
    // TODO: Replace testingRootStore with a AttestationTrustStore that has the necessary official PAA roots available
    const chip::Credentials::AttestationTrustStore * testingRootStore = chip::Credentials::GetTestAttestationTrustStore();
    SetDeviceAttestationVerifier(GetDefaultDACVerifier(testingRootStore));

    CHIP_ERROR err = sOperationalCredentialsIssuer.Initialize(sStorageDelegate);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err);

    err = sFabricStorage.Initialize(&sStorageDelegate);
    VerifyOrReturnError(err == CHIP_NO_ERROR, err);

    chip::Crypto::P256Keypair ephemeralKey;
    err = ephemeralKey.Initialize();
    VerifyOrReturnError(err == CHIP_NO_ERROR, err);

    chip::Platform::ScopedMemoryBuffer<uint8_t> noc;
    ReturnErrorCodeIf(!noc.Alloc(kMaxCHIPDERCertLength), CHIP_ERROR_NO_MEMORY);
    MutableByteSpan nocSpan(noc.Get(), kMaxCHIPDERCertLength);

    chip::Platform::ScopedMemoryBuffer<uint8_t> icac;
    ReturnErrorCodeIf(!icac.Alloc(kMaxCHIPDERCertLength), CHIP_ERROR_NO_MEMORY);
    MutableByteSpan icacSpan(icac.Get(), kMaxCHIPDERCertLength);

    chip::Platform::ScopedMemoryBuffer<uint8_t> rcac;
    ReturnErrorCodeIf(!rcac.Alloc(kMaxCHIPDERCertLength), CHIP_ERROR_NO_MEMORY);
    MutableByteSpan rcacSpan(rcac.Get(), kMaxCHIPDERCertLength);

    ReturnErrorOnFailure(sOperationalCredentialsIssuer.GenerateNOCChainAfterValidation(localDeviceId, 0, ephemeralKey.Pubkey(),
                                                                                       rcacSpan, icacSpan, nocSpan));

    FactoryInitParams factoryParams;
    factoryParams.fabricStorage            = &sFabricStorage;
    factoryParams.fabricIndependentStorage = &sStorageDelegate;

    SetupParams initParams;
    initParams.storageDelegate                = &sStorageDelegate;
    initParams.deviceAddressUpdateDelegate    = &sDeviceAddressUpdateDelegate;
    initParams.pairingDelegate                = &sPairingDelegate;
    initParams.operationalCredentialsDelegate = &sOperationalCredentialsIssuer;
    initParams.operationalKeypair             = &ephemeralKey;
    initParams.controllerRCAC                 = rcacSpan;
    initParams.controllerICAC                 = icacSpan;
    initParams.controllerNOC                  = nocSpan;

    ReturnErrorOnFailure(DeviceControllerFactory::GetInstance().Init(factoryParams));
    ReturnErrorOnFailure(DeviceControllerFactory::GetInstance().SetupCommissioner(initParams, sController));

    chip::rpc::RPCServer::GetInstance()->controller = &sController;

    return CHIP_NO_ERROR;
}

} // namespace

int main()
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    err = chip::Platform::MemoryInit();
    VerifyOrDieWithMsg(err == CHIP_NO_ERROR, Controller, "Failed to init memory: %s", chip::ErrorStr(err));

    err = chip::DeviceLayer::PlatformMgr().InitChipStack();
    VerifyOrDieWithMsg(err == CHIP_NO_ERROR, DeviceLayer, "Failed to initialize CHIP stack: platform init failed: %s",
                       chip::ErrorStr(err));

    err = InitController();
    VerifyOrDieWithMsg(err == CHIP_NO_ERROR, Controller, "Failed to init device controller: %s", chip::ErrorStr(err));

    int result   = pthread_create(&sPlatformMainThread, nullptr, PlatformMainLoop, nullptr);
    int tmpErrno = errno;

    VerifyOrDieWithMsg(result == 0, DeviceLayer, "Failed to initialize CHIP stack: pthread_create failed: %s", strerror(tmpErrno));

    result = rpc::Init();
    VerifyOrDieWithMsg(result == 0, DeviceLayer, "Failed to initialize CHIP RPC server: %s", strerror(tmpErrno));

    pthread_join(sPlatformMainThread, nullptr);

    return 0;
}
