
#include <BSGraphics/BSGraphicsRenderer.h>

struct BSInputDeviceManager;

void (*BSInputDeviceManager_PollInputDevices)(BSInputDeviceManager*, float) = nullptr;

void Hook_BSInputDeviceManager_PollInputDevices(BSInputDeviceManager* inputDeviceMgr, float afDelta)
{
    if (!BSGraphics::GetMainWindow()->IsForeground())
        return;

    BSInputDeviceManager_PollInputDevices(inputDeviceMgr, afDelta);
}

static TiltedPhoques::Initializer s_initInputDeviceManager([]() {
    const VersionDbPtr<void> pollInputDevices(68617);

    BSInputDeviceManager_PollInputDevices = 
        static_cast<decltype(BSInputDeviceManager_PollInputDevices)>(pollInputDevices.GetPtr());

    TP_HOOK_IMMEDIATE(&BSInputDeviceManager_PollInputDevices, &Hook_BSInputDeviceManager_PollInputDevices);
});
