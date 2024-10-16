#pragma once

#include "DInputHook.hpp"
#include "simpleini/SimpleIni.h"

#include <dinput.h>

namespace TiltedPhoques
{
struct OverlayClient;
struct OverlayRenderHandler;
} // namespace TiltedPhoques

struct InputService;
struct DebugService;
struct OverlayService;

struct KeyPressEvent;

namespace fs = std::filesystem;

#define DEFAULT_UI_KEY    \
    {                     \
        L"F2",            \
        {                 \
            VK_F2, DIK_F2 \
        }                 \
    }
#define DEFAULT_DEBUG_KEY \
    {                     \
        L"F3",            \
        {                 \
            VK_F3, DIK_F3 \
        }                 \
    }

/**
 * @brief Handles keybinds
 *
 * @details Loads a config during construction. The key will not actually be set until it is pressed or loaded from config.
 * This is due to needing separate VirtualKey and DirectInput keycodes that can only be determined after being pressed. This service
 * needs to stay connected to DInputHook OnKeyPress Signal due to handling UI toggling (sometimes misses otherwise)
 * Does not currently support mouse buttons.
 */
struct KeybindService
{
    struct KeyCodes
    {
        enum
        {
            Error = 0
        };

        // VirtualKey
        uint16_t vkKeyCode = Error;
        // DirectInput
        unsigned long diKeyCode = Error;

        friend bool operator==(const KeyCodes& acLhs, const KeyCodes& acRhs) noexcept { return acLhs.vkKeyCode == acRhs.vkKeyCode && acLhs.diKeyCode == acRhs.diKeyCode; }
    };

    using Key = std::pair<TiltedPhoques::WString, KeyCodes>;

    struct Config
    {
        static constexpr char kConfigPathName[] = "config";
        static constexpr char kKeybindsFileName[] = "keybinds.ini";

        bool Create() noexcept;
        bool Save() const noexcept;
        bool Load() noexcept;

        bool SetKey(const wchar_t* acpKey, const wchar_t* acpValue, const wchar_t* acpDescription = nullptr) noexcept;
        bool SetKeyCodes(const wchar_t* acpConfigKey, const KeybindService::KeyCodes& acKeyCodes) noexcept;
        KeyCodes GetKeyCodes(const wchar_t* acpKey) const noexcept;

        CSimpleIniW ini{};
        fs::path path{};
    };

    KeybindService(entt::dispatcher& aDispatcher, InputService& aInputService, DebugService& aDebugService);
    ~KeybindService();

    TP_NOCOPYMOVE(KeybindService);

    bool BindUIKey(const uint16_t& acKeyCode) noexcept;
    bool BindDebugKey(const uint16_t& acKeyCode) noexcept;

    const Key& GetUIKey() const noexcept { return m_uiKey; }
    const Key& GetDebugKey() const noexcept { return m_debugKey; }

    const Config& GetConfig() const noexcept { return m_config; }

private:
    void OnDirectInputKeyPress(const unsigned long& acKeyCode) noexcept;
    void OnVirtualKeyKeyPress(const KeyPressEvent& acKeyCode) noexcept;

    Key MakeKey(const uint16_t& acKeyCode) noexcept;

    void InitializeKeys(bool aLoadDefaults) noexcept;
    bool SetDebugKey(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, const TiltedPhoques::WString& acKeyName, const bool& acLoadFromConfig = false) noexcept;
    bool SetUIKey(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, const TiltedPhoques::WString& acKeyName, const bool& acLoadFromConfig = false) noexcept;
    void HandleKeybind(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, const bool& acLoadFromConfig = false) noexcept;
    bool CanToggleDebug(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode) const noexcept;
    bool DoKeysMatch(const KeybindService::Key& acLeftKey, const KeybindService::Key& acRightKey) const noexcept;

    static TiltedPhoques::WString ConvertToWString(const TiltedPhoques::String& acString) noexcept;
    static TiltedPhoques::String ConvertToString(const TiltedPhoques::WString& acString) noexcept;
    static uint16_t ResolveVkKeyModifier(const uint16_t& acKeyCode) noexcept;

    void SetupConfig() noexcept;

    entt::dispatcher& m_dispatcher;
    InputService& m_inputService;
    DebugService& m_debugService;
    TiltedPhoques::DInputHook* m_pInputHook;

    uint16_t m_keyCode{0};

    bool m_uiKeybindConfirmed{false};
    bool m_debugKeybindConfirmed{false};

    size_t m_dikKeyPressConnection{0};
    entt::scoped_connection m_vkKeyPressConnection{};

    Config m_config{};

    // Keys are not actually "set" until they are pressed from both UI and ingame (to tie together VirtualKeys with DirectInput)
    // or loaded from config
    Key m_uiKey{};
    Key m_debugKey{};

    // Keys with custom names
    const TiltedPhoques::Map<Key::first_type, Key::second_type>& m_modifiedKeys{
        {L"Backspace", {VK_BACK, DIK_BACK}},
        {L"Tab", {VK_TAB, DIK_TAB}},
        {L"Enter", {VK_RETURN, DIK_RETURN}},
        {L"LSHIFT", {VK_LSHIFT, DIK_LSHIFT}},
        {L"RSHIFT", {VK_RSHIFT, DIK_RSHIFT}},
        {L"LCTRL", {VK_LCONTROL, DIK_LCONTROL}},
        {L"RCTRL", {VK_RCONTROL, DIK_RCONTROL}},
        {L"LALT", {VK_LMENU, DIK_LMENU}},
        {L"RALT", {VK_RMENU, DIK_RMENU}},
        {L"Pause", {VK_PAUSE, DIK_PAUSE}},
        {L"Caps Lock", {VK_CAPITAL, DIK_CAPSLOCK}},
        {L"IME Kana mode", {VK_KANA, DIK_KANA}},
        {L"IME Kanji mode", {VK_KANJI, DIK_KANJI}},
        {L"Esc", {VK_ESCAPE, DIK_ESCAPE}},
        {L"IME convert", {VK_CONVERT, DIK_CONVERT}},
        {L"IME nonconvert", {VK_NONCONVERT, DIK_NOCONVERT}},
        {L"Space", {VK_SPACE, DIK_SPACE}},
        {L"Page Up", {VK_PRIOR, DIK_PRIOR}},
        {L"Page Down", {VK_NEXT, DIK_NEXT}},
        {L"End", {VK_END, DIK_END}},
        {L"Home", {VK_HOME, DIK_HOME}},
        {L"Arrow Left", {VK_LEFT, DIK_LEFT}},
        {L"Arrow Up", {VK_UP, DIK_UP}},
        {L"Arrow Right", {VK_RIGHT, DIK_RIGHT}},
        {L"Arrow Down", {VK_DOWN, DIK_DOWN}},
        {L"Ins", {VK_INSERT, DIK_INSERT}},
        {L"Del", {VK_DELETE, DIK_DELETE}},
        {L"LWIN", {VK_LWIN, DIK_LWIN}},
        {L"RWIN", {VK_RWIN, DIK_RWIN}},
        {L"Applications", {VK_APPS, DIK_APPS}},
        {L"Sleep", {VK_SLEEP, DIK_SLEEP}},
        {L"NUMPAD 0", {VK_NUMPAD0, DIK_NUMPAD0}},
        {L"NUMPAD 1", {VK_NUMPAD1, DIK_NUMPAD1}},
        {L"NUMPAD 2", {VK_NUMPAD2, DIK_NUMPAD2}},
        {L"NUMPAD 3", {VK_NUMPAD3, DIK_NUMPAD3}},
        {L"NUMPAD 4", {VK_NUMPAD4, DIK_NUMPAD4}},
        {L"NUMPAD 5", {VK_NUMPAD5, DIK_NUMPAD5}},
        {L"NUMPAD 6", {VK_NUMPAD6, DIK_NUMPAD6}},
        {L"NUMPAD 7", {VK_NUMPAD7, DIK_NUMPAD7}},
        {L"NUMPAD 8", {VK_NUMPAD8, DIK_NUMPAD8}},
        {L"NUMPAD 9", {VK_NUMPAD9, DIK_NUMPAD9}},
        {L"NUMPAD *", {VK_MULTIPLY, DIK_MULTIPLY}},
        {L"NUMPAD +", {VK_ADD, DIK_ADD}},
        {L"NUMPAD -", {VK_SUBTRACT, DIK_SUBTRACT}},
        {L"NUMPAD .", {VK_DECIMAL, DIK_DECIMAL}},
        {L"NUMPAD /", {VK_DIVIDE, DIK_DIVIDE}},
        {L"F1", {VK_F1, DIK_F1}},
        {L"F2", {VK_F2, DIK_F2}},
        {L"F3", {VK_F3, DIK_F3}},
        {L"F4", {VK_F4, DIK_F4}},
        {L"F5", {VK_F5, DIK_F5}},
        {L"F6", {VK_F6, DIK_F6}},
        {L"F7", {VK_F7, DIK_F7}},
        {L"F8", {VK_F8, DIK_F8}},
        {L"F9", {VK_F9, DIK_F9}},
        {L"F10", {VK_F10, DIK_F10}},
        {L"F11", {VK_F11, DIK_F11}},
        {L"F12", {VK_F12, DIK_F12}},
        {L"F13", {VK_F13, DIK_F13}},
        {L"F14", {VK_F14, DIK_F14}},
        {L"F15", {VK_F15, DIK_F15}},
        {L"Num Lock", {VK_NUMLOCK, DIK_NUMLOCK}},
        {L"ScrLk", {VK_SCROLL, DIK_SCROLL}},
        {L"Browser Back", {VK_BROWSER_BACK, DIK_WEBBACK}},
        {L"Browser Forward", {VK_BROWSER_FORWARD, DIK_WEBFORWARD}},
        {L"Browser Refresh", {VK_BROWSER_REFRESH, DIK_WEBREFRESH}},
        {L"Browser Stop", {VK_BROWSER_STOP, DIK_WEBSTOP}},
        {L"Browser Search", {VK_BROWSER_SEARCH, DIK_WEBSEARCH}},
        {L"Browser Favorites", {VK_BROWSER_FAVORITES, DIK_WEBFAVORITES}},
        {L"Browser Home", {VK_BROWSER_HOME, DIK_WEBHOME}},
        {L"Volume Mute", {VK_VOLUME_MUTE, DIK_MUTE}},
        {L"Volume Down", {VK_VOLUME_DOWN, DIK_VOLUMEDOWN}},
        {L"Volume Up", {VK_VOLUME_UP, DIK_VOLUMEUP}},
        {L"Next Track", {VK_MEDIA_NEXT_TRACK, DIK_NEXTTRACK}},
        {L"Previous Track", {VK_MEDIA_PREV_TRACK, DIK_PREVTRACK}},
        {L"Stop Media", {VK_MEDIA_STOP, DIK_MEDIASTOP}},
        {L"Start Mail", {VK_LAUNCH_MAIL, DIK_MAIL}},
        {L"Select Media", {VK_LAUNCH_MEDIA_SELECT, DIK_MEDIASELECT}}

    };
};
