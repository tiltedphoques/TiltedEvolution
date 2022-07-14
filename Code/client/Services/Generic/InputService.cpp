#include <TiltedOnlinePCH.h>

#include <Services/InputService.h>
#include <Services/OverlayService.h>

#include <OverlayApp.hpp>

#include <DInputHook.hpp>
#include <WindowsHook.hpp>

#include <include/internal/cef_types.h>
#include <Services/ImguiService.h>
#include <Services/DiscordService.h>
#include <World.h>

#include "Games/Skyrim/Interface/MenuControls.h"

static OverlayService* s_pOverlay = nullptr;

void ForceKillAllInput()
{
#if TP_SKYRIM
    MenuControls::GetInstance()->SetToggle(false);
#else
// TODO! Crash the project so we notice
    int* t = nullptr;
    *t = 42;
#endif
}

uint32_t GetCefModifiers(uint16_t aVirtualKey)
{
    uint32_t modifiers = EVENTFLAG_NONE;

    if (GetKeyState(VK_MENU) & 0x8000)
    {
        modifiers |= EVENTFLAG_ALT_DOWN;
    }

    if (GetKeyState(VK_CONTROL) & 0x8000)
    {
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    }

    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    }

    if (GetKeyState(VK_LBUTTON) & 0x8000)
    {
        modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    }

    if (GetKeyState(VK_RBUTTON) & 0x8000)
    {
        modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
    }

    if (GetKeyState(VK_MBUTTON) & 0x8000)
    {
        modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    }

    if (GetKeyState(VK_CAPITAL) & 1)
    {
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;
    }

    if (GetKeyState(VK_NUMLOCK) & 1)
    {
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    }

    if (aVirtualKey)
    {
        if (aVirtualKey == VK_RCONTROL || aVirtualKey == VK_RMENU || aVirtualKey == VK_RSHIFT)
        {
            modifiers |= EVENTFLAG_IS_RIGHT;
        }
        else if (aVirtualKey == VK_LCONTROL || aVirtualKey == VK_LMENU || aVirtualKey == VK_LSHIFT)
        {
            modifiers |= EVENTFLAG_IS_LEFT;
        }
        else if (aVirtualKey >= VK_NUMPAD0 && aVirtualKey <= VK_DIVIDE)
        {
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        }
    }

    return modifiers;
}

// remember to update this when updating toggle keys
bool IsToggleKey(int aKey) noexcept
{
    return aKey == VK_RCONTROL || aKey == VK_F2;
}

bool IsDisableKey(int aKey) noexcept
{
    return aKey == VK_ESCAPE;
}

void SetUIActive(OverlayService& aOverlay, auto apRenderer, bool aActive)
{
#if defined(TP_SKYRIM)
     TiltedPhoques::DInputHook::Get().SetEnabled(!aActive);
     aOverlay.SetActive(!aActive);
#else
     pRenderer->SetVisible(!aActive);
#endif

    // Ensures the game is actually loaded, in case the initial event was sent too early
    aOverlay.SetVersion(BUILD_COMMIT);
    aOverlay.GetOverlayApp()->ExecuteAsync("enterGame");

    apRenderer->SetCursorVisible(!aActive);

    // This is to disable the Windows cursor
    while (ShowCursor(FALSE) >= 0)
        ;
}

void ProcessKeyboard(uint16_t aKey, uint16_t aScanCode, cef_key_event_type_t aType, bool aE0, bool aE1)
{
    if (aType != KEYEVENT_CHAR)
    {
        if (!aKey || aKey == 255)
        {
            return;
        }

        if (aKey == VK_SHIFT)
        {
            aKey = static_cast<uint16_t>(MapVirtualKey(aScanCode, MAPVK_VSC_TO_VK_EX));
        }
        else if (aKey == VK_NUMLOCK)
        {
            aScanCode = static_cast<uint16_t>(MapVirtualKey(aKey, MAPVK_VK_TO_VSC) | 0x100);
        }

        if (aE1)
        {
            if (aKey == VK_PAUSE)
            {
                aScanCode = 0x45;
            }
            else
            {
                aScanCode = static_cast<uint16_t>(MapVirtualKey(aKey, MAPVK_VK_TO_VSC));
            }
        }

        if (aE0)
        {
            switch (aKey)
            {
            case VK_CONTROL:
                aKey = VK_RCONTROL;
                break;
            case VK_MENU:
                aKey = VK_RMENU;
                break;
            case VK_RETURN:
                aKey = VK_SEPARATOR;
                break;
            }
        }
        else
        {
            switch (aKey)
            {
            case VK_CONTROL:
                aKey = VK_LCONTROL;
                break;
            case VK_MENU:
                aKey = VK_LMENU;
                break;
            case VK_INSERT:
                aKey = VK_NUMPAD0;
                break;
            case VK_DELETE:
                aKey = VK_DECIMAL;
                break;
            case VK_HOME:
                aKey = VK_NUMPAD7;
                break;
            case VK_END:
                aKey = VK_NUMPAD1;
                break;
            case VK_PRIOR:
                aKey = VK_NUMPAD9;
                break;
            case VK_NEXT:
                aKey = VK_NUMPAD3;
                break;
            case VK_LEFT:
                aKey = VK_NUMPAD4;
                break;
            case VK_RIGHT:
                aKey = VK_NUMPAD6;
                break;
            case VK_UP:
                aKey = VK_NUMPAD8;
                break;
            case VK_DOWN:
                aKey = VK_NUMPAD2;
                break;
            case VK_CLEAR:
                aKey = VK_NUMPAD5;
                break;
            }
        }
    }

    auto& overlay = *s_pOverlay;

    const auto pApp = overlay.GetOverlayApp();
    if (!pApp)
        return;

    const auto pClient = pApp->GetClient();
    if (!pClient)
        return;

    const auto pRenderer = pClient->GetOverlayRenderHandler();
    if (!pRenderer)
        return;

    auto active = overlay.GetActive();

    spdlog::debug("ProcessKey, type: {}, key: {}, active: {}", aType, aKey, active);

    if (IsToggleKey(aKey) || IsDisableKey(aKey))
    {
        if (active && IsDisableKey(aKey))
        {
            active = true;
        }
        if (!overlay.GetInGame())
        {
            TiltedPhoques::DInputHook::Get().SetEnabled(false);
        }
        // This is really hacky, but when the input hook is enabled initially, it does not propogate the KEYDOWN event
        else if (aType == KEYEVENT_KEYDOWN || (aType == KEYEVENT_KEYUP && !active))
        {
            SetUIActive(overlay, pRenderer, active);
        }
    }
    else if (active)
    {
        pApp->InjectKey(aType, GetCefModifiers(aKey), aKey, aScanCode);
    }
}

void ProcessMouseMove(uint16_t aX, uint16_t aY)
{
    auto& overlay = *s_pOverlay;

    const auto pApp = overlay.GetOverlayApp();
    if (!pApp)
        return;

    const auto pClient = pApp->GetClient();
    if (!pClient)
        return;

    const auto pRenderer = pClient->GetOverlayRenderHandler();
    if (!pRenderer)
        return;

    const auto active = overlay.GetActive();

    if (active)
    {
        pApp->InjectMouseMove(aX, aY, GetCefModifiers(0));
    }
}

void ProcessMouseButton(uint16_t aX, uint16_t aY, cef_mouse_button_type_t aButton, bool aDown)
{   
    auto& overlay = *s_pOverlay;

    const auto pApp = overlay.GetOverlayApp();
    if (!pApp)
        return;

    const auto pClient = pApp->GetClient();
    if (!pClient)
        return;

    const auto pRenderer = pClient->GetOverlayRenderHandler();
    if (!pRenderer)
        return;

    const auto active = overlay.GetActive();

    if (active)
    {
        pApp->InjectMouseButton(aX, aY, aButton, !aDown, GetCefModifiers(0));
    }
}

void ProcessMouseWheel(uint16_t aX, uint16_t aY, int16_t aZ)
{
    auto& overlay = *s_pOverlay;

    const auto pApp = overlay.GetOverlayApp();
    if (!pApp)
        return;

    const auto pClient = pApp->GetClient();
    if (!pClient)
        return;

    const auto pRenderer = pClient->GetOverlayRenderHandler();
    if (!pRenderer)
        return;

    const auto active = overlay.GetActive();

    if (active)
    {
        pApp->InjectMouseWheel(aX, aY, aZ, GetCefModifiers(0));
    }
}

LRESULT CALLBACK InputService::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const auto pApp = s_pOverlay->GetOverlayApp();
    if (!pApp)
        return 0;

    const auto pClient = pApp->GetClient();
    if (!pClient)
        return 0;

    const auto pRenderer = pClient->GetOverlayRenderHandler();
    if (!pRenderer)
        return 0;

    auto &discord = World::Get().ctx().at<DiscordService>();
    discord.WndProcHandler(hwnd, uMsg, wParam, lParam);

    const bool active = s_pOverlay->GetActive();
    if (active)
    {
        auto& imgui = World::Get().ctx().at<ImguiService>();
        imgui.WndProcHandler(hwnd, uMsg, wParam, lParam);
    }

#if TP_FALLOUT4
    POINTER_FALLOUT4(uint8_t, s_viewportLock, 0x3846670);
    *s_viewportLock = isVisible ? 1 : 0;
#endif

    POINT position;

    GetCursorPos(&position);
    ScreenToClient(GetActiveWindow(), &position);

    ProcessMouseMove(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y));

    if (uMsg == WM_INPUT)
    {
        RAWINPUT input;
        UINT size = sizeof(RAWINPUT);

        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &input, &size, sizeof(RAWINPUTHEADER));

        if (active)
        {
            auto& imgui = World::Get().ctx().at<ImguiService>();
            imgui.RawInputHandler(input);
        }

        if (input.header.dwType == RIM_TYPEKEYBOARD)
        {
            const auto keyboard = input.data.keyboard;

            ProcessKeyboard(keyboard.VKey, keyboard.MakeCode, keyboard.Flags & RI_KEY_BREAK ? KEYEVENT_KEYUP : KEYEVENT_KEYDOWN, keyboard.Flags & RI_KEY_E0, keyboard.Flags & RI_KEY_E1);
        }
        else if (input.header.dwType == RIM_TYPEMOUSE)
        {
            const auto mouse = input.data.mouse;

            if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
            {
                ProcessMouseWheel(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y), (int16_t)mouse.usButtonData);
            }

            if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
            {
                ProcessMouseButton(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y), MBT_LEFT, true);
            }

            if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
            {
                ProcessMouseButton(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y), MBT_LEFT, false);
            }

            if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
            {
                ProcessMouseButton(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y), MBT_RIGHT, true);
            }

            if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
            {
                ProcessMouseButton(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y), MBT_RIGHT, false);
            }

            if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
            {
                ProcessMouseButton(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y), MBT_MIDDLE, true);
            }

            if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
            {
                ProcessMouseButton(static_cast<uint16_t>(position.x), static_cast<uint16_t>(position.y), MBT_MIDDLE, false);
            }
        }
    }
    else if (uMsg == WM_CHAR)
    {
        ProcessKeyboard(static_cast<uint16_t>(wParam), (lParam >> 16) & 0xFF, KEYEVENT_CHAR, false, false);
    }

#if defined(TP_FALLOUT)
    // Fallout specific code to disable input
    if (isVisible)
        return 1;
#endif

    return 0;
}

InputService::InputService(OverlayService& aOverlay) noexcept
{
    s_pOverlay = &aOverlay;
}

InputService::~InputService() noexcept
{
    s_pOverlay = nullptr;
}

