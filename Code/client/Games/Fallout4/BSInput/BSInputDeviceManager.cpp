
#include <BSGraphics/BSGraphicsRenderer.h>

struct BSInputDeviceManager;

void (*BSInputDeviceManager_PollInputDevices)(BSInputDeviceManager*, float) = nullptr;

void Hook_BSInputDeviceManager_PollInputDevices(BSInputDeviceManager* inputDeviceMgr, float afDelta)
{
    if (!BSGraphics::GetMainWindow()->IsForeground())
        return;

    BSInputDeviceManager_PollInputDevices(inputDeviceMgr, afDelta);
}

BOOL ClipCursor_H(const RECT*)
{
    RECT realRect{};
    GetWindowRect(BSGraphics::GetMainWindow()->hWnd, &realRect);
    return ClipCursor(&realRect);
}

static TiltedPhoques::Initializer s_initInputDeviceManager([]() {
    const VersionDbPtr<uint8_t> pollInputDevices(1328120);

    BSInputDeviceManager_PollInputDevices =
        static_cast<decltype(BSInputDeviceManager_PollInputDevices)>(pollInputDevices.GetPtr());

    
    TP_HOOK_IMMEDIATE(&BSInputDeviceManager_PollInputDevices, &Hook_BSInputDeviceManager_PollInputDevices);

    // TODO: move to a proper place.
    const VersionDbPtr<uint8_t> updateGameCursor(847267);
    // disable the game being able to force the OS cursor
    TiltedPhoques::Put<uint8_t>(updateGameCursor.Get() + 0xB1, 0xEB);

    // no game scaled window bounds.
    TiltedPhoques::Put<uint16_t>(updateGameCursor.Get() + 0xE1, 0xE990);

    // proper coords.
    TiltedPhoques::PutCall(updateGameCursor.Get() + 0x27A, ClipCursor_H);
    TiltedPhoques::Put<uint8_t>(updateGameCursor.Get() + 0x27A + 5, 0x90);
});
