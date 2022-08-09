
#include <BSGraphics/BSGraphicsRenderer.h>

struct BSInputDeviceManager;

void (*BSInputDeviceManager_PollInputDevices)(BSInputDeviceManager*, float) = nullptr;

void Hook_BSInputDeviceManager_PollInputDevices(BSInputDeviceManager* inputDeviceMgr, float afDelta)
{
    if (!BSGraphics::GetMainWindow()->IsForeground())
        return;

    BSInputDeviceManager_PollInputDevices(inputDeviceMgr, afDelta);
}

BOOL __stdcall H_ClipCursor(const RECT* lpRect)
{
    int h = lpRect->bottom - lpRect->top;
    int w = lpRect->right - lpRect->left;

    return FALSE;
}

void H_GetClipCursor(LPRECT lpRect)
{
    GetWindowRect(BSGraphics::GetMainWindow()->hWnd, lpRect);
    ClipCursor(lpRect);
}

BOOL ClipCursor_H(const RECT*)
{
    RECT realRect{};
    GetWindowRect(BSGraphics::GetMainWindow()->hWnd, &realRect);
    return ClipCursor(&realRect);
}

static TiltedPhoques::Initializer s_initInputDeviceManager([]() {
    const VersionDbPtr<void> pollInputDevices(1328120);

    BSInputDeviceManager_PollInputDevices =
        static_cast<decltype(BSInputDeviceManager_PollInputDevices)>(pollInputDevices.GetPtr());

    TP_HOOK_IMMEDIATE(&BSInputDeviceManager_PollInputDevices, &Hook_BSInputDeviceManager_PollInputDevices);

    // disable the game being able to force the OS cursor
    TiltedPhoques::Put<uint8_t>(0x140D38BF1, 0xEB);

    // no game scaled window bounds.
    TiltedPhoques::Put<uint16_t>(0x140D38C21, 0xE990);

    // proper coords.
    TiltedPhoques::PutCall(0x140D38DBA, ClipCursor_H);
    TiltedPhoques::Put<uint8_t>(0x140D38DBA + 5, 0x90);
});
