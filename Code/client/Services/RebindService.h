#pragma once

#include <dinput.h>

struct InputService;

struct RebindService
{
    struct KeyCodes
    {
        enum { Error = -1 };

        int32_t vkKeyCode = Error;
        int32_t diKeyCode = Error;
    };

    using Key = std::pair<TiltedPhoques::String, KeyCodes>;

    RebindService(InputService& aInputService);
    ~RebindService() = default;

    TP_NOCOPYMOVE(RebindService);

    TiltedPhoques::Map<Key::first_type, Key::second_type> GetKeys() const noexcept { return m_keys; }

    // UI Key
    const Key& GetUIKey() const noexcept { return m_pUiKey; }
    bool SetUIKey(std::shared_ptr<RebindService::Key> apKey) noexcept;
    bool SetUIKeyFromKeyName(const TiltedPhoques::String& acKeyName) noexcept;
    bool SetUIKeyFromVKKeyCode(int32_t aKeyCode) noexcept;
    bool SetUIKeyFromDIKeyCode(int32_t aKeyCode) noexcept;

    // Debug Key
    const Key& GetDebugKey() const noexcept { return m_pDebugKey; }

    // General Key
    std::shared_ptr<RebindService::Key> GetKeyFromName(const TiltedPhoques::String& acKeyName) const noexcept;
    std::shared_ptr<RebindService::Key> GetKeyFromVKKeyCode(int32_t aKeyCode) const noexcept;
    int32_t GetVKKeyCode(const TiltedPhoques::String& aKeyName) const noexcept;
    std::shared_ptr<RebindService::Key> GetKeyFromDIKeyCode(int32_t aKeyCode) const noexcept;
    int32_t GetDIKeyCode(const TiltedPhoques::String& aKeyName) const noexcept;
    bool IsValidKey(const TiltedPhoques::String& acKeyName) const noexcept { return m_keys.find(acKeyName) != m_keys.end(); }

private:
    Key m_pUiKey = {"F2", {VK_F2, DIK_F2}};
    Key m_pDebugKey = {"F3", {VK_F3, DIK_F3}};
    
    InputService& m_inputService;

    const TiltedPhoques::Map<Key::first_type, Key::second_type> m_keys
    {
        {"", {KeyCodes::Error, KeyCodes::Error}},
        {"A", {0x41, DIK_A}},
        {"B", {0x42, DIK_B}},
        {"C", {0x43, DIK_C}},
        {"D", {0x44, DIK_D}},
        {"E", {0x45, DIK_E}},
        {"F", {0x46, DIK_F}},
        {"G", {0x47, DIK_G}},
        {"H", {0x48, DIK_H}},
        {"I", {0x49, DIK_I}},
        {"J", {0x4A, DIK_J}},
        {"K", {0x4B, DIK_K}},
        {"L", {0x4C, DIK_L}},
        {"M", {0x4D, DIK_M}},
        {"N", {0x4E, DIK_N}},
        {"O", {0x4F, DIK_O}},
        {"P", {0x50, DIK_P}},
        {"Q", {0x51, DIK_Q}},
        {"R", {0x52, DIK_R}},
        {"S", {0x53, DIK_S}},
        {"T", {0x54, DIK_T}},
        {"U", {0x55, DIK_U}},
        {"V", {0x56, DIK_V}},
        {"W", {0x57, DIK_W}},
        {"X", {0x58, DIK_X}},
        {"Y", {0x59, DIK_Y}},
        {"Z", {0x5A, DIK_Z}},
        {"0", {0x30, DIK_0}},
        {"1", {0x31, DIK_1}},
        {"2", {0x32, DIK_2}},
        {"3", {0x33, DIK_3}},
        {"4", {0x34, DIK_4}},
        {"5", {0x35, DIK_5}},
        {"6", {0x36, DIK_6}},
        {"7", {0x37, DIK_7}},
        {"8", {0x38, DIK_8}},
        {"9", {0x39, DIK_9}},
        /*{"MOUSE1", {VK_LBUTTON, DIMOUSE_BUTTON0}},
        {"MOUSE2", {VK_RBUTTON, DIMOUSE_BUTTON1}},
        {"MOUSE3", {VK_MBUTTON, DIMOUSE_BUTTON2}},
        {"MOUSE4", {VK_XBUTTON1, DIMOUSE_BUTTON3}},
        {"MOUSE5", {VK_XBUTTON2, DIMOUSE_BUTTON4}},*/
        {"Backspace", {VK_BACK, DIK_BACK}},
        {"Tab", {VK_TAB, DIK_TAB}},
        {"Enter", {VK_RETURN, DIK_RETURN}},
        {"LSHIFT", {VK_LSHIFT, DIK_LSHIFT}},
        {"RSHIFT", {VK_RSHIFT, DIK_RSHIFT}},
        {"LCTRL", {VK_LCONTROL, DIK_LCONTROL}},
        {"RCTRL", {VK_RCONTROL, DIK_RCONTROL}},
        {"LALT", {VK_LMENU, DIK_LMENU}},
        {"RALT", {VK_RMENU, DIK_RMENU}},
        {"Pause", {VK_PAUSE, DIK_PAUSE}},
        {"Caps Lock", {VK_CAPITAL, DIK_CAPITAL}},
        {"IME Kana mode", {VK_KANA, DIK_KANA}},
        {"IME Kanji mode", {VK_KANJI, DIK_KANJI}},
        {"Esc", {VK_ESCAPE, DIK_ESCAPE}},
        {"IME convert", {VK_CONVERT, DIK_CONVERT}},
        {"IME nonconvert", {VK_NONCONVERT, DIK_NOCONVERT}},
        {"Space", {VK_SPACE, DIK_SPACE}},
        {"Page Up", {VK_PRIOR, DIK_PRIOR}},
        {"Page Down", {VK_NEXT, DIK_NEXT}},
        {"End", {VK_END, DIK_END}},
        {"Home", {VK_HOME, DIK_HOME}},
        {"Arrow Left", {VK_LEFT, DIK_LEFT}},
        {"Arrow Up", {VK_UP, DIK_UP}},
        {"Arrow Right", {VK_RIGHT, DIK_RIGHT}},
        {"Arrow Down", {VK_DOWN, DIK_DOWN}},
        {"Ins", {VK_INSERT, DIK_INSERT}},
        {"Del", {VK_DELETE, DIK_DELETE}},
        {"LWIN", {VK_LWIN, DIK_LWIN}},
        {"RWIN", {VK_RWIN, DIK_RWIN}},
        {"Applications", {VK_APPS, DIK_APPS}},
        {"Sleep", {VK_SLEEP, DIK_SLEEP}},
        {"NUMPAD 0", {VK_NUMPAD0, DIK_NUMPAD0}},
        {"NUMPAD 1", {VK_NUMPAD1, DIK_NUMPAD1}},
        {"NUMPAD 2", {VK_NUMPAD2, DIK_NUMPAD2}},
        {"NUMPAD 3", {VK_NUMPAD3, DIK_NUMPAD3}},
        {"NUMPAD 4", {VK_NUMPAD4, DIK_NUMPAD4}},
        {"NUMPAD 5", {VK_NUMPAD5, DIK_NUMPAD5}},
        {"NUMPAD 6", {VK_NUMPAD6, DIK_NUMPAD6}},
        {"NUMPAD 7", {VK_NUMPAD7, DIK_NUMPAD7}},
        {"NUMPAD 8", {VK_NUMPAD8, DIK_NUMPAD8}},
        {"NUMPAD 9", {VK_NUMPAD9, DIK_NUMPAD9}},
        {"NUMPAD *", {VK_MULTIPLY, DIK_MULTIPLY}},
        {"NUMPAD +", {VK_ADD, DIK_ADD}},
        {"NUMPAD -", {VK_SUBTRACT, DIK_SUBTRACT}},
        {"NUMPAD .", {VK_DECIMAL, DIK_DECIMAL}},
        {"NUMPAD /", {VK_DIVIDE, DIK_DIVIDE}},
        {"F1", {VK_F1, DIK_F1}},
        {"F2", {VK_F2, DIK_F2}},
        {"F3", {VK_F3, DIK_F3}},
        {"F4", {VK_F4, DIK_F4}},
        {"F5", {VK_F5, DIK_F5}},
        {"F6", {VK_F6, DIK_F6}},
        {"F7", {VK_F7, DIK_F7}},
        {"F8", {VK_F8, DIK_F8}},
        {"F9", {VK_F9, DIK_F9}},
        {"F10", {VK_F10, DIK_F10}},
        {"F11", {VK_F11, DIK_F11}},
        {"F12", {VK_F12, DIK_F12}},
        {"F13", {VK_F13, DIK_F13}},
        {"F14", {VK_F14, DIK_F14}},
        {"F15", {VK_F15, DIK_F15}},
        {"Num Lock", {VK_NUMLOCK, DIK_NUMLOCK}},
        {"ScrLk", {VK_SCROLL, DIK_SCROLL}},
        {"Browser Back", {VK_BROWSER_BACK, DIK_WEBBACK}},
        {"Browser Forward", {VK_BROWSER_FORWARD, DIK_WEBFORWARD}},
        {"Browser Refresh", {VK_BROWSER_REFRESH, DIK_WEBREFRESH}},
        {"Browser Stop", {VK_BROWSER_STOP, DIK_WEBSTOP}},
        {"Browser Search", {VK_BROWSER_SEARCH, DIK_WEBSEARCH}},
        {"Browser Favorites", {VK_BROWSER_FAVORITES, DIK_WEBFAVORITES}},
        {"Browser Stard and Home", {VK_BROWSER_HOME, DIK_WEBHOME}},
        {"Volume Mute", {VK_VOLUME_MUTE, DIK_MUTE}},
        {"Volume Down", {VK_VOLUME_DOWN, DIK_VOLUMEDOWN}},
        {"Volume Up", {VK_VOLUME_UP, DIK_VOLUMEUP}},
        {"Next Track", {VK_MEDIA_NEXT_TRACK, DIK_NEXTTRACK}},
        {"Previous Track", {VK_MEDIA_PREV_TRACK, DIK_PREVTRACK}},
        {"Stop Media", {VK_MEDIA_STOP, DIK_MEDIASTOP}},
        {"Play/Pause Media", {VK_MEDIA_PLAY_PAUSE, DIK_PLAYPAUSE}},
        {"Start Mail", {VK_LAUNCH_MAIL, DIK_MAIL}},
        {"Select Media", {VK_LAUNCH_MEDIA_SELECT, DIK_MEDIASELECT}},
        {"=", {VK_OEM_PLUS, DIK_EQUALS}},
        {",", {VK_OEM_COMMA, DIK_COMMA}},
        {"-", {VK_OEM_MINUS, DIK_MINUS}},
        {".", {VK_OEM_PERIOD, DIK_PERIOD}},
        {";", {VK_OEM_1, DIK_SEMICOLON}},
        {"/", {VK_OEM_2, DIK_SLASH}},
        {"`", {VK_OEM_3, DIK_GRAVE}},
        {"[", {VK_OEM_4, DIK_LBRACKET}},
        {"\\", {VK_OEM_5, DIK_BACKSLASH}},
        {"]", {VK_OEM_6, DIK_RBRACKET}},
        {"'", {VK_OEM_7, DIK_APOSTROPHE}},
    };
};
