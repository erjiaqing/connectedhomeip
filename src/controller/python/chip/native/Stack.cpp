#include "Stack.h"

#include <controller/CHIPDeviceControllerFactory.h>
#include <controller/python/chip/native/StackGlobals.h>
#include <controller/python/chip/native/StorageDelegate.h>

#include <lib/support/CodeUtils.h>
#include <platform/TestOnlyCommissionableDataProvider.h>

using namespace chip;
using namespace chip::Python;
using namespace chip::Controller;
using namespace chip::Python::Globals;

namespace chip {
namespace Python {
namespace Globals {
Credentials::PersistentStorageOpCertStore sPersistentStorageOpCertStore;
Credentials::GroupDataProviderImpl sGroupDataProvider;
} // namespace Globals
} // namespace Python
} // namespace chip

PyChipError pychip_StackInit(pychip_StorageAdapter * pyStorageAdapter, bool enableServerInteractions)
{
    VerifyOrDie(pyStorageAdapter != nullptr);
    StorageAdapter * storageAdapter = static_cast<StorageAdapter *>(pyStorageAdapter);

    FactoryInitParams factoryParams;

    factoryParams.fabricIndependentStorage = storageAdapter;

    sGroupDataProvider.SetStorageDelegate(storageAdapter);
    ReturnErrorOnFailure(sGroupDataProvider.Init());
    factoryParams.groupDataProvider = &sGroupDataProvider;

    PyReturnErrorOnFailure(sPersistentStorageOpCertStore.Init(storageAdapter));
    factoryParams.opCertStore = &sPersistentStorageOpCertStore;

    factoryParams.enableServerInteractions = enableServerInteractions;

    // Hack needed due to the fact that DnsSd server uses the CommissionableDataProvider even
    // when never starting commissionable advertising. This will not be used but prevents
    // null pointer dereferences.
    static chip::DeviceLayer::TestOnlyCommissionableDataProvider TestOnlyCommissionableDataProvider;
    chip::DeviceLayer::SetCommissionableDataProvider(&TestOnlyCommissionableDataProvider);

    ReturnErrorOnFailure(DeviceControllerFactory::GetInstance().Init(factoryParams));

    //
    // In situations where all the controller instances get shutdown, the entire stack is then also
    // implicitly shutdown. In the REPL, users can create such a situation by manually shutting down
    // controllers (for example, when they call ChipReplStartup::LoadFabricAdmins multiple times). In
    // that situation, momentarily, the stack gets de-initialized. This results in further interactions with
    // the stack being dangerous (and in fact, causes crashes).
    //
    // This retain call ensures the stack doesn't get de-initialized in the REPL.
    //
    DeviceControllerFactory::GetInstance().RetainSystemState();

    //
    // Finally, start up the main Matter thread. Any further interactions with the stack
    // will now need to happen on the Matter thread, OR protected with the stack lock.
    //
    PyReturnErrorOnFailure(chip::DeviceLayer::PlatformMgr().StartEventLoopTask());

    return CHIP_NO_ERROR;
}

PyChipError pychip_StackShutdown()
{
    ChipLogError(Controller, "Shutting down the stack...");

    //
    // Let's stop the Matter thread, and wait till the event loop has stopped.
    //
    PyReturnErrorOnFailure(chip::DeviceLayer::PlatformMgr().StopEventLoopTask());

    //
    // There is the symmetric call to match the Retain called at stack initialization
    // time. This will release all resources (if there are no other controllers active).
    //
    DeviceControllerFactory::GetInstance().ReleaseSystemState();

    DeviceControllerFactory::GetInstance().Shutdown();

    return CHIP_NO_ERROR;
}

pychip_StorageAdapter * pychip_Storage_InitializeStorageAdapter(PyObject * context, pychip_Storage_SyncSetKeyValueCb setCb,
                                                                pychip_Storage_SetGetKeyValueCb getCb,
                                                                pychip_Storage_SyncDeleteKeyValueCb deleteCb)
{
    return new StorageAdapter(context, setCb, getCb, deleteCb);
}

void pychip_Storage_ShutdownAdapter(pychip_StorageAdapter * storageAdapter)
{
    StorageAdapter * adapter = static_cast<StorageAdapter *>(storageAdapter);
    delete adapter;
}

struct __attribute__((packed)) pychip_CommonStackInitParams
{
    uint32_t mBluetoothAdapterId;
};

void pychip_CommonStackShutdown()
{
#if 0 //
      // We cannot actually call this because the destructor for the MdnsContexts singleton on Darwin only gets called
      // on termination of the program, and that unfortunately makes a bunch of Platform::MemoryFree calls.
      //
    Platform::MemoryShutdown();
#endif
}

/**
 * Function to artifically cause a crash to happen
 * that can be used in place of os.exit() in Python so that
 * when run through GDB, you'll get a backtrace of what happened.
 */
void pychip_CauseCrash()
{
    uint8_t * ptr = nullptr;
    *ptr          = 0;
}
