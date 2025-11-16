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
struct MagicService;

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
#define DEFAULT_REVEAL_PLAYERS_KEY \
    {                              \
        L"F4",                     \
        {                          \
            VK_F4, DIK_F4          \
        }                          \
    }

/**
 * @brief Handles keybinds
 *
 * @details Loads a config during construction. The key will not actually be set until it is pressed or loaded from config.
 * This is due to needing separate VirtualKey and DirectInput keycodes that can only be determined after being pressed. This should only handle
 * keybind-related functionalities.
 * Does not currently support mouse buttons.
 */
struct KeybindService
{
    enum Keybind : uint8_t
    {
        None = 1 << 0,
        UI = 1 << 1,
        Debug = 1 << 2,
        RevealPlayers = 1 << 3,
        All = UI | Debug | RevealPlayers
    };

    struct KeyCodes
    {
        enum : uint8_t
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

    KeybindService(entt::dispatcher& aDispatcher, InputService& aInputService, DebugService& aDebugService, MagicService& aMagicService);
    ~KeybindService();

    TP_NOCOPYMOVE(KeybindService);

    const Config& GetConfig() const noexcept { return m_config; }

    bool GetTextInputFocus() const noexcept { return m_isTextInputFocused; }
    void SetTextInputFocus(bool aTextInputFocused) noexcept { m_isTextInputFocused = aTextInputFocused; }

    bool BindKey(const Keybind& acKeyType, const uint16_t& acNewKeyCode) noexcept;
    const Key& GetKey(const Keybind& acKeyType) const noexcept;

    void ResetKeybinds(const Keybind& acKeyType) noexcept;

private:
    // Handles DirectInput-related keybind inputs
    // Handling them elsewhere could cause missed keystate changes
    void OnDirectInputKeyPress(const unsigned long& acKeyCode) noexcept;
    // Handles VirtualKey-related keybind inputs
    // Handling them elsewhere could cause missed keystate changes
    void OnVirtualKeyKeyPress(const KeyPressEvent& acKeyCode) noexcept;

    Key MakeKey(const uint16_t& acKeyCode) noexcept;

    bool SetKeyValues(Key& acKeyToChange, const Key& acKey) noexcept;
    bool SetKey(const Keybind& acKeybind, const Key& acKey, bool acLoadFromConfig = false) noexcept;
    bool HandleSetUI(const Key& acKey, bool acLoadFromConfig = false) noexcept;
    bool HandleSetDebug(const Key& acKey, bool acLoadFromConfig = false) noexcept;
    bool HandleSetRevealPlayers(const Key& acKey, bool acLoadFromConfig = false) noexcept;
    bool HandleBind(const Keybind& acKeybind, const uint16_t& acNewKeyCode) noexcept;

    void InitializeKeys(bool aLoadDefaults) noexcept;
    void HandleKeybind(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, bool acLoadFromConfig = false) noexcept;
    bool DoKeysMatch(const Key& acLeftKey, const Key& acRightKey) const noexcept;

    bool CanToggleDebug(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode) const noexcept;
    bool CanRevealOtherPlayers(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode) const noexcept;

    static TiltedPhoques::WString ConvertToWString(const TiltedPhoques::String& acString) noexcept;
    static TiltedPhoques::String ConvertToString(const TiltedPhoques::WString& acString) noexcept;
    static TiltedPhoques::WString KeybindService::ToUpper(const TiltedPhoques::WString& aString) noexcept;
    static uint16_t ResolveVkKeyModifier(const uint16_t& acKeyCode) noexcept;

    void SetupConfig() noexcept;
    void CheckForDuplicates() noexcept;

    entt::dispatcher& m_dispatcher;
    InputService& m_inputService;
    DebugService& m_debugService;
    MagicService& m_magicService;
    TiltedPhoques::DInputHook* m_pInputHook;

    bool m_isTextInputFocused{false};

    uint16_t m_keyCode{0};

    bool m_uiKeybindConfirmed{false};
    bool m_debugKeybindConfirmed{false};
    bool m_revealPlayersKeybindConfirmed{false};

    size_t m_dikKeyPressConnection{0};
    entt::scoped_connection m_vkKeyPressConnection{};

    Config m_config{};

    // Keys are not actually "set" until they are pressed from both UI and ingame (to tie together VirtualKeys with DirectInput)
    // or loaded from config
    Key m_uiKey{};
    Key m_debugKey{};
    Key m_revealPlayersKey{};

    const TiltedPhoques::Map<Key::first_type, Key::second_type>& m_customKeyNames{
        {L"BACKSPACE", {VK_BACK, DIK_BACK}},
        {L"TAB", {VK_TAB, DIK_TAB}},
        {L"ENTER", {VK_RETURN, DIK_RETURN}},
        {L"LSHIFT", {VK_LSHIFT, DIK_LSHIFT}},
        {L"RSHIFT", {VK_RSHIFT, DIK_RSHIFT}},
        {L"LCTRL", {VK_LCONTROL, DIK_LCONTROL}},
        {L"RCTRL", {VK_RCONTROL, DIK_RCONTROL}},
        {L"LALT", {VK_LMENU, DIK_LMENU}},
        {L"RALT", {VK_RMENU, DIK_RMENU}},
        {L"PAUSE", {VK_PAUSE, DIK_PAUSE}},
        {L"CAPS LOCK", {VK_CAPITAL, DIK_CAPSLOCK}},
        {L"IME KANA MODE", {VK_KANA, DIK_KANA}},
        {L"IME KANJI MODE", {VK_KANJI, DIK_KANJI}},
        {L"ESC", {VK_ESCAPE, DIK_ESCAPE}},
        {L"IME CONVERT", {VK_CONVERT, DIK_CONVERT}},
        {L"IME NONCONVERT", {VK_NONCONVERT, DIK_NOCONVERT}},
        {L"SPACE", {VK_SPACE, DIK_SPACE}},
        {L"PAGE UP", {VK_PRIOR, DIK_PRIOR}},
        {L"PAGE DOWN", {VK_NEXT, DIK_NEXT}},
        {L"END", {VK_END, DIK_END}},
        {L"HOME", {VK_HOME, DIK_HOME}},
        {L"ARROW LEFT", {VK_LEFT, DIK_LEFT}},
        {L"ARROW UP", {VK_UP, DIK_UP}},
        {L"ARROW RIGHT", {VK_RIGHT, DIK_RIGHT}},
        {L"ARROW DOWN", {VK_DOWN, DIK_DOWN}},
        {L"INS", {VK_INSERT, DIK_INSERT}},
        {L"DEL", {VK_DELETE, DIK_DELETE}},
        {L"LWIN", {VK_LWIN, DIK_LWIN}},
        {L"RWIN", {VK_RWIN, DIK_RWIN}},
        {L"APPLICATIONS", {VK_APPS, DIK_APPS}},
        {L"SLEEP", {VK_SLEEP, DIK_SLEEP}},
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
        {L"NUM LOCK", {VK_NUMLOCK, DIK_NUMLOCK}},
        {L"SCRLK", {VK_SCROLL, DIK_SCROLL}},
        {L"BROWSER BACK", {VK_BROWSER_BACK, DIK_WEBBACK}},
        {L"BROWSER FORWARD", {VK_BROWSER_FORWARD, DIK_WEBFORWARD}},
        {L"BROWSER REFRESH", {VK_BROWSER_REFRESH, DIK_WEBREFRESH}},
        {L"BROWSER STOP", {VK_BROWSER_STOP, DIK_WEBSTOP}},
        {L"BROWSER SEARCH", {VK_BROWSER_SEARCH, DIK_WEBSEARCH}},
        {L"BROWSER FAVORITES", {VK_BROWSER_FAVORITES, DIK_WEBFAVORITES}},
        {L"BROWSER HOME", {VK_BROWSER_HOME, DIK_WEBHOME}},
        {L"VOLUME MUTE", {VK_VOLUME_MUTE, DIK_MUTE}},
        {L"VOLUME DOWN", {VK_VOLUME_DOWN, DIK_VOLUMEDOWN}},
        {L"VOLUME UP", {VK_VOLUME_UP, DIK_VOLUMEUP}},
        {L"NEXT TRACK", {VK_MEDIA_NEXT_TRACK, DIK_NEXTTRACK}},
        {L"PREVIOUS TRACK", {VK_MEDIA_PREV_TRACK, DIK_PREVTRACK}},
        {L"STOP MEDIA", {VK_MEDIA_STOP, DIK_MEDIASTOP}},
        {L"START MAIL", {VK_LAUNCH_MAIL, DIK_MAIL}},
        {L"SELECT MEDIA", {VK_LAUNCH_MEDIA_SELECT, DIK_MEDIASELECT}}

    };
};
