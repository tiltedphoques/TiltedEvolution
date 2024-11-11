#include "Services/KeybindService.h"

#include "DInputHook.hpp"
#include "OverlayApp.hpp"

#include "Events/KeyPressEvent.h"

KeybindService::KeybindService(entt::dispatcher& aDispatcher, InputService& aInputService, DebugService& aDebugService)
    : m_dispatcher(aDispatcher)
    , m_inputService(aInputService)
    , m_debugService(aDebugService)
{
    m_pInputHook = &TiltedPhoques::DInputHook::Get();

    m_dikKeyPressConnection = m_pInputHook->OnKeyPress.Connect(std::bind(&KeybindService::OnDirectInputKeyPress, this, std::placeholders::_1));
    m_vkKeyPressConnection = m_dispatcher.sink<KeyPressEvent>().connect<&KeybindService::OnVirtualKeyKeyPress>(this);

    SetupConfig();
}

KeybindService::~KeybindService()
{
    m_pInputHook = nullptr;
}

void KeybindService::SetupConfig() noexcept
{
    m_config.ini.SetUnicode(true);

    const auto cModPath = TiltedPhoques::GetPath();
    m_config.path = cModPath / Config::kConfigPathName / Config::kKeybindsFileName;

    if (!exists(cModPath / Config::kConfigPathName))
    {
        create_directory(cModPath / Config::kConfigPathName);
    }

    if (!exists(m_config.path))
    {
        spdlog::debug("{} not found, creating...", Config::kKeybindsFileName);

        if (m_config.Create())
        {
            InitializeKeys(true);
        }
    }
    else
    {
        spdlog::debug("{} found, loading...", Config::kKeybindsFileName);

        if (m_config.Load())
        {
            spdlog::debug("Successfully loaded {}", Config::kKeybindsFileName);

            InitializeKeys(false);
        }
        else
        {
            spdlog::warn("{} Failed to load {}", __FUNCTION__, Config::kKeybindsFileName);
        }
    }

    spdlog::info(L"UI Key: {} | Debug Key: {}", m_uiKey.first.c_str(), m_debugKey.first.c_str());
}

void KeybindService::InitializeKeys(bool aLoadDefaults) noexcept
{
    if (aLoadDefaults)
    {
        m_uiKey = DEFAULT_UI_KEY;
        m_debugKey = DEFAULT_DEBUG_KEY;
    }
    else
    {
        m_uiKey.first = m_config.ini.GetValue(L"UI", L"sUiKey", L"F2");
        m_debugKey.first = m_config.ini.GetValue(L"UI", L"sDebugKey", L"F3");

        // If attempting to bind both to one key, change debug to F3 (as key is less likely to be used)
        // If UI is set to F3, reset UI key to ensure keys are different
        if (m_uiKey.first == m_debugKey.first)
        {
            spdlog::warn("{} Cannot bind both Debug UI and STR UI to same key, resetting key", __FUNCTION__);

            if (m_uiKey.first != L"F3")
                m_debugKey = DEFAULT_DEBUG_KEY;
            else
                m_uiKey = DEFAULT_UI_KEY;
        }

        m_uiKey.second = m_config.GetKeyCodes(L"ui");
        m_debugKey.second = m_config.GetKeyCodes(L"debug");
    }

    HandleKeybind(m_uiKey.second.vkKeyCode, m_uiKey.second.diKeyCode, true);
    HandleKeybind(m_debugKey.second.vkKeyCode, m_debugKey.second.diKeyCode, true);
}

bool KeybindService::SetDebugKey(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, const TiltedPhoques::WString& acKeyName, const bool& acLoadFromConfig) noexcept
{
    if (m_debugKey.first == acKeyName)
    {
        if (acVkKeyCode != KeyCodes::Error)
            m_debugKey.second.vkKeyCode = acVkKeyCode;

        if (acDiKeyCode != KeyCodes::Error)
            m_debugKey.second.diKeyCode = acDiKeyCode;

        if (!acKeyName.empty())
            m_debugKey.first = acKeyName;

        m_debugService.SetDebugKey(TiltedPhoques::MakeShared<Key>(m_debugKey));

        m_debugKeybindConfirmed = m_debugKey.second.vkKeyCode != KeyCodes::Error && m_debugKey.second.diKeyCode != KeyCodes::Error;
    }
    else
    {
        // Keybind may have been changed via config file, start reconciling the key
        m_debugKey.second = {KeyCodes::Error, KeyCodes::Error};
        m_debugKeybindConfirmed = false;
    }

    if (!acLoadFromConfig)
        return m_config.SetKey(L"sDebugKey", m_debugKey.first.c_str()) && m_config.SetKeyCodes(L"debug", m_debugKey.second);

    return true;
}

bool KeybindService::SetUIKey(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, const TiltedPhoques::WString& acKeyName, const bool& acLoadFromConfig) noexcept
{
    if (m_uiKey.first == acKeyName)
    {
        if (acVkKeyCode != KeyCodes::Error)
            m_uiKey.second.vkKeyCode = acVkKeyCode;

        if (acDiKeyCode != KeyCodes::Error)
            m_uiKey.second.diKeyCode = acDiKeyCode;

        if (!acKeyName.empty())
            m_uiKey.first = acKeyName;

        m_inputService.SetUIKey(TiltedPhoques::MakeShared<Key>(m_uiKey));
        m_pInputHook->SetToggleKeys({m_uiKey.second.diKeyCode});

        m_uiKeybindConfirmed = m_uiKey.second.vkKeyCode != KeyCodes::Error && m_uiKey.second.diKeyCode != KeyCodes::Error;
    }
    else
    {
        // Keybind may have been changed via config file, start reconciling the key
        m_uiKey.second = {KeyCodes::Error, KeyCodes::Error};
        m_uiKeybindConfirmed = false;
    }

    // Don't toggle UI if loading from config
    if (!acLoadFromConfig)
    {
        m_pInputHook->SetEnabled(true);
        m_inputService.Toggle(acVkKeyCode, MapVirtualKey(m_uiKey.second.vkKeyCode, MAPVK_VK_TO_VSC), KEYEVENT_CHAR);

        return m_config.SetKey(L"sUiKey", m_uiKey.first.c_str()) && m_config.SetKeyCodes(L"ui", m_uiKey.second);
    }

    return true;
}

bool KeybindService::BindUIKey(const uint16_t& acKeyCode) noexcept
{
    if (acKeyCode == m_debugKey.second.vkKeyCode || acKeyCode == m_uiKey.second.vkKeyCode)
        return false;

    m_uiKeybindConfirmed = false;

    const auto& cKey = MakeKey(acKeyCode);
    m_uiKey = cKey;

    return SetUIKey(cKey.second.vkKeyCode, cKey.second.diKeyCode, cKey.first);
}

bool KeybindService::BindDebugKey(const uint16_t& acKeyCode) noexcept
{
    if (acKeyCode == m_debugKey.second.vkKeyCode || acKeyCode == m_uiKey.second.vkKeyCode)
        return false;

    m_debugKeybindConfirmed = false;

    const auto& cKey = MakeKey(acKeyCode);
    m_debugKey = cKey;

    return SetDebugKey(cKey.second.vkKeyCode, cKey.second.diKeyCode, cKey.first);
}

void KeybindService::OnVirtualKeyKeyPress(const KeyPressEvent& acEvent) noexcept
{
    m_keyCode = acEvent.VirtualKey >= 0x10 && acEvent.VirtualKey <= 0x12 ? ResolveVkKeyModifier(acEvent.VirtualKey) : acEvent.VirtualKey;

    if (!m_debugKeybindConfirmed || !m_uiKeybindConfirmed)
    {
        HandleKeybind(m_keyCode, KeyCodes::Error);
    }

    if (CanToggleDebug(m_keyCode, KeyCodes::Error))
    {
        m_debugService.DebugPressed();
    }
}

void KeybindService::OnDirectInputKeyPress(const unsigned long& acKeyCode) noexcept
{
    if (!m_uiKeybindConfirmed || !m_debugKeybindConfirmed)
    {
        HandleKeybind(KeyCodes::Error, acKeyCode);
    }

    // DebugService would sometimes miss debug key's state change so it is handled here
    if (CanToggleDebug(KeyCodes::Error, acKeyCode))
    {
        m_debugService.DebugPressed();
    }
}

void KeybindService::HandleKeybind(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, const bool& acLoadFromConfig) noexcept
{
    m_keyCode = acVkKeyCode;

    if (m_keyCode == KeyCodes::Error && !acLoadFromConfig)
    {
        // TODO: this can still miss but is the lesser of two evils...
        for (uint16_t key = 256; key > 0; key--)
        {
            if (GetAsyncKeyState(key) & 0x8000)
            {
                m_keyCode = key;
                break;
            }
        }
    }

    wchar_t keyChar = static_cast<wchar_t>(MapVirtualKeyW(m_keyCode, MAPVK_VK_TO_CHAR));

    const TiltedPhoques::WString& cKeyName = {static_cast<wchar_t>(toupper(keyChar))};
    const KeybindService::Key& cKey = {cKeyName, {acVkKeyCode, acDiKeyCode}};
    auto pModKey = std::ranges::find_if(m_modifiedKeys, [&](const KeybindService::Key& acKey) { return acKey.second.vkKeyCode == m_keyCode || acKey.second.diKeyCode == acDiKeyCode; });

    // Key has custom name
    if (pModKey != m_modifiedKeys.end())
    {
        m_keyCode = pModKey->second.vkKeyCode;

        // UI key pressed
        if (DoKeysMatch(*pModKey, m_uiKey) && !m_uiKeybindConfirmed)
        {
            SetUIKey(m_keyCode, acDiKeyCode, pModKey->first, acLoadFromConfig);
        }
        // Debug key pressed
        else if (DoKeysMatch(*pModKey, m_debugKey) && !m_debugKeybindConfirmed)
        {
            SetDebugKey(m_keyCode, acDiKeyCode, pModKey->first);
        }
    }
    // No custom key name, UI key was pressed
    else if (DoKeysMatch(cKey, m_uiKey) && !m_uiKeybindConfirmed)
    {
        SetUIKey(m_keyCode, acDiKeyCode, cKeyName, acLoadFromConfig);
    }
    // No custom key name, Debug key was pressed
    else if (DoKeysMatch(cKey, m_debugKey) && !m_debugKeybindConfirmed)
    {
        SetDebugKey(m_keyCode, acDiKeyCode, cKeyName);
    }
}

KeybindService::Key KeybindService::MakeKey(const uint16_t& acKeyCode) noexcept
{
    m_keyCode = acKeyCode;

    TiltedPhoques::WString newName = {static_cast<wchar_t>(MapVirtualKeyW(acKeyCode, MAPVK_VK_TO_CHAR))};
    auto pModKey = std::ranges::find_if(m_modifiedKeys, [&](const KeybindService::Key& acKey) { return acKey.second.vkKeyCode == m_keyCode; });

    if (pModKey != m_modifiedKeys.end())
    {
        if (pModKey != m_modifiedKeys.end())
        {
            newName = pModKey->first;
            m_keyCode = pModKey->second.vkKeyCode;
        }
    }

    return Key{newName, {m_keyCode, KeyCodes::Error}};
}

bool KeybindService::CanToggleDebug(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode) const noexcept
{
    return (acVkKeyCode != KeyCodes::Error && acVkKeyCode == m_debugKey.second.vkKeyCode) || (acDiKeyCode != KeyCodes::Error && acDiKeyCode == m_debugKey.second.diKeyCode);
}

bool KeybindService::DoKeysMatch(const KeybindService::Key& acLeftKey, const KeybindService::Key& acRightKey) const noexcept
{
    return (!acLeftKey.first.empty() && acLeftKey.first == acRightKey.first) || (acLeftKey.second.vkKeyCode != KeyCodes::Error && acLeftKey.second.vkKeyCode == acRightKey.second.vkKeyCode) ||
           (acLeftKey.second.diKeyCode != KeyCodes::Error && acLeftKey.second.diKeyCode == acRightKey.second.diKeyCode);
}

TiltedPhoques::WString KeybindService::ConvertToWString(const TiltedPhoques::String& acString) noexcept
{
    int nChars = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, acString.data(), static_cast<int>(acString.length()), nullptr, 0);

    TiltedPhoques::WString wstrTo;
    if (nChars > 0)
    {
        wstrTo.resize(nChars);

        if (MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, acString.data(), static_cast<int>(acString.length()), &wstrTo[0], nChars))
        {
            return wstrTo;
        }
    }

    return {};
}

TiltedPhoques::String KeybindService::ConvertToString(const TiltedPhoques::WString& aString) noexcept
{
    int nChars = WideCharToMultiByte(CP_ACP, 0, aString.data(), static_cast<int>(aString.length()), nullptr, 0, nullptr, nullptr);

    TiltedPhoques::String strTo{};
    if (nChars > 0)
    {
        strTo.resize(nChars);

        if (WideCharToMultiByte(CP_ACP, 0, aString.data(), static_cast<int>(aString.length()), &strTo[0], nChars, nullptr, nullptr))
        {
            return strTo;
        }
    }

    return {};
}

uint16_t KeybindService::ResolveVkKeyModifier(const uint16_t& acKeyCode) noexcept
{
    switch (acKeyCode)
    {
    case VK_SHIFT:
        if (GetAsyncKeyState(VK_LSHIFT) & 0x80)
            return VK_LSHIFT;
        else
            return VK_RSHIFT;
    case VK_CONTROL:
        if (GetAsyncKeyState(VK_LCONTROL) & 0x80)
            return VK_LCONTROL;
        else
            return VK_RCONTROL;
    case VK_MENU:
        if (GetAsyncKeyState(VK_LMENU) & 0x80)
            return VK_LMENU;
        else
            return VK_RMENU;
    default: return acKeyCode;
    }
}

bool KeybindService::Config::Create() noexcept
{
    if (this->ini.SaveFile(this->path.c_str(), true) != SI_OK)
    {
        spdlog::warn("{} Failed to create {}, using defaults instead", __FUNCTION__, kKeybindsFileName);
        return false;
    }

    spdlog::info("Successfully created {}", kKeybindsFileName);

    this->ini.SetValue(L"UI", L"sUiKey", L"F2");
    this->ini.SetValue(L"UI", L"sDebugKey", L"F3");

    const TiltedPhoques::WString cUiKeyCodes{std::to_wstring(VK_F2) + L"," + std::to_wstring(DIK_F2)};
    const TiltedPhoques::WString cDebugKeyCodes{std::to_wstring(VK_F3) + L"," + std::to_wstring(DIK_F3)};

    this->ini.SetValue(L"Internal", L"ui", cUiKeyCodes.c_str());
    this->ini.SetValue(L"Internal", L"debug", cDebugKeyCodes.c_str());

    if (!this->Save())
    {
        spdlog::warn("Failed to save {}", kKeybindsFileName);
        return false;
    }

    return true;
}

bool KeybindService::Config::Save() const noexcept
{
    return this->ini.SaveFile(this->path.c_str(), true) == SI_OK;
}

bool KeybindService::Config::Load() noexcept
{
    return this->ini.LoadFile(this->path.c_str()) == SI_OK;
}

bool KeybindService::Config::SetKey(const wchar_t* acpKey, const wchar_t* acpValue, const wchar_t* acpDescription) noexcept
{
    this->ini.SetValue(L"UI", acpKey, acpValue, acpDescription);

    return this->Save();
}

bool KeybindService::Config::SetKeyCodes(const wchar_t* acpConfigKey, const KeybindService::KeyCodes& acKeyCodes) noexcept
{
    const TiltedPhoques::WString cKeyString{std::to_wstring(acKeyCodes.vkKeyCode) + L"," + std::to_wstring(acKeyCodes.diKeyCode)};

    this->ini.SetValue(L"Internal", acpConfigKey, cKeyString.c_str());

    return this->Save();
}

KeybindService::KeyCodes KeybindService::Config::GetKeyCodes(const wchar_t* acpKey) const noexcept
{
    const TiltedPhoques::WString& cKeyString = this->ini.GetValue(L"Internal", acpKey);

    const auto& vkKeyCode = static_cast<uint16_t>(std::stoi(cKeyString.substr(cKeyString.find_first_of(L"=") + 1, cKeyString.find_first_of(L",")).c_str()));
    const auto& diKeyCode = std::stoul(cKeyString.substr(cKeyString.find_first_of(L",") + 1).c_str());

    return {vkKeyCode, diKeyCode};
}
