#pragma once

#include "DInputHook.hpp"
#include "simpleini/SimpleIni.h"

#include <dinput.h>

struct InputService;

namespace fs = std::filesystem;

/**
 * @brief Handles keybinds
 *
 * @details Loads a config during construction, a key name will be set. The key will not actually be set until it is pressed.
 * This is due to needing separate VirtualKey and DirectInput keycodes that can only be determined after being pressed. The service only
 * connects to the DInputHook OnKeyPress Signal when it is checking for keycodes, disconnected otherwise.
 */
struct KeybindService
{
    struct Config
    {
        static constexpr char kConfigPathName[] = "config";
        static constexpr char kKeybindsFileName[] = "keybinds.ini";

        bool Create() noexcept;
        bool Save() const noexcept;
        bool Load() noexcept;

        bool SetKey(const char* acpSection, const char* acpKey, const char* acpValue, const char* acpDescription = nullptr) noexcept;

        CSimpleIniA ini{};
        fs::path path{};
    };


    struct KeyCodes
    {
        enum { Error = 0 };

        int32_t vkKeyCode = Error;
        unsigned long diKeyCode = Error;
    };

    using Key = std::pair<TiltedPhoques::String, KeyCodes>;

    KeybindService(InputService& aInputService);
    ~KeybindService() = default;

    TP_NOCOPYMOVE(KeybindService);

    const Key& GetUIKey() const noexcept { return m_uiKey; }
    const Key& GetDebugKey() const noexcept { return m_debugKey; }
    bool SetDebugKey(std::shared_ptr<KeybindService::Key> apKey) noexcept;
    void BindNewKey(int32_t aKeyCode) noexcept;
    void OnDirectInputKeyPress(unsigned long aKeyCode);
    wchar_t ConvertToUnicode(int32_t aKeyCode) noexcept;

    // Config
    void SetupConfig() noexcept;

    Config config{};

private:
    InputService& m_inputService;
    TiltedPhoques::DInputHook* m_inputHook;

    unsigned long m_lastKeyPressed{0};
    int32_t m_keyCode{0};
    bool m_newKeyPressed{false};
    bool m_bindActive{false};

    // Flag for checking if key is being rebound during OnDirectInputKeyPress
    bool m_keybindConfirmed{false};
    bool m_convertedToUnicode{false};

    size_t m_keyPressConnection;

    // Keys not actually "set" until they are pressed from both UI and ingame
    Key m_uiKey{};
    Key m_debugKey{};

    // Keys that have custom names
    TiltedPhoques::Map<Key::first_type, int32_t> m_modifiedKeys
    {
        {"Backspace", VK_BACK},
        {"Tab", VK_TAB},
        {"Enter", VK_RETURN},
        {"LSHIFT", VK_LSHIFT},
        {"RSHIFT", VK_RSHIFT},
        {"LCTRL", VK_LCONTROL},
        {"RCTRL", VK_RCONTROL},
        {"LALT", VK_LMENU},
        {"RALT", VK_RMENU},
        {"Pause", VK_PAUSE},
        {"Caps Lock", VK_CAPITAL},
        {"IME Kana mode", VK_KANA},
        {"IME Kanji mode", VK_KANJI},
        {"Esc", VK_ESCAPE},
        {"IME convert", VK_CONVERT},
        {"IME nonconvert", VK_NONCONVERT},
        {"Space", VK_SPACE},
        {"Page Up", VK_PRIOR},
        {"Page Down", VK_NEXT},
        {"End", VK_END},
        {"Home", VK_HOME},
        {"Arrow Left", VK_LEFT},
        {"Arrow Up", VK_UP},
        {"Arrow Right", VK_RIGHT},
        {"Arrow Down", VK_DOWN},
        {"Ins", VK_INSERT},
        {"Del", VK_DELETE},
        {"LWIN", VK_LWIN},
        {"RWIN", VK_RWIN},
        {"Applications", VK_APPS},
        {"Sleep", VK_SLEEP},
        {"NUMPAD 0", VK_NUMPAD0},
        {"NUMPAD 1", VK_NUMPAD1},
        {"NUMPAD 2", VK_NUMPAD2},
        {"NUMPAD 3", VK_NUMPAD3},
        {"NUMPAD 4", VK_NUMPAD4},
        {"NUMPAD 5", VK_NUMPAD5},
        {"NUMPAD 6", VK_NUMPAD6},
        {"NUMPAD 7", VK_NUMPAD7},
        {"NUMPAD 8", VK_NUMPAD8},
        {"NUMPAD 9", VK_NUMPAD9},
        {"NUMPAD *", VK_MULTIPLY},
        {"NUMPAD +", VK_ADD},
        {"NUMPAD -", VK_SUBTRACT},
        {"NUMPAD .", VK_DECIMAL},
        {"NUMPAD /", VK_DIVIDE},
        {"F1", VK_F1},
        {"F2", VK_F2},
        {"F3", VK_F3},
        {"F4", VK_F4},
        {"F5", VK_F5},
        {"F6", VK_F6},
        {"F7", VK_F7},
        {"F8", VK_F8},
        {"F9", VK_F9},
        {"F10", VK_F10},
        {"F11", VK_F11},
        {"F12", VK_F12},
        {"F13", VK_F13},
        {"F14", VK_F14},
        {"F15", VK_F15},
        {"Num Lock", VK_NUMLOCK},
        {"ScrLk", VK_SCROLL},
        {"Browser Back", VK_BROWSER_BACK},
        {"Browser Forward", VK_BROWSER_FORWARD},
        {"Browser Refresh", VK_BROWSER_REFRESH},
        {"Browser Stop", VK_BROWSER_STOP},
        {"Browser Search", VK_BROWSER_SEARCH},
        {"Browser Favorites", VK_BROWSER_FAVORITES},
        {"Browser Stard and Home", VK_BROWSER_HOME},
        {"Volume Mute", VK_VOLUME_MUTE},
        {"Volume Down", VK_VOLUME_DOWN},
        {"Volume Up", VK_VOLUME_UP},
        {"Next Track", VK_MEDIA_NEXT_TRACK},
        {"Previous Track", VK_MEDIA_PREV_TRACK},
        {"Stop Media", VK_MEDIA_STOP},
        {"Play/Pause Media", VK_MEDIA_PLAY_PAUSE},
        {"Start Mail", VK_LAUNCH_MAIL},
        {"Select Media", VK_LAUNCH_MEDIA_SELECT}

    };
};
