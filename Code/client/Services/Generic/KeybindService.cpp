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

    const auto modPath = TiltedPhoques::GetPath();
    m_config.path = modPath / Config::kConfigPathName / Config::kKeybindsFileName;

    if (!exists(modPath / Config::kConfigPathName))
    {
        create_directory(modPath / Config::kConfigPathName);
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

    spdlog::info("UI key: {} | Debug key: {}", m_uiKey.first.c_str(), m_debugKey.first.c_str());
}

void KeybindService::InitializeKeys(bool aLoadDefaults) noexcept
{
    if (aLoadDefaults)
    {
        m_uiKey = {"F2", {VK_F2, DIK_F2}};
        m_debugKey = {"F3", {VK_F3, DIK_F3}};

        m_inputService.SetUIKey(TiltedPhoques::MakeShared<Key>(m_uiKey));
        m_debugService.SetDebugKey(TiltedPhoques::MakeShared<Key>(m_debugKey));
    }
    else
    {
        m_uiKey.first = m_config.ini.GetValue("UI", "sUiKey", "F2");
        m_debugKey.first = m_config.ini.GetValue("UI", "sDebugKey", "F3");

        m_inputService.SetUIKey(TiltedPhoques::MakeShared<Key>(m_uiKey));
        m_debugService.SetDebugKey(TiltedPhoques::MakeShared<Key>(m_debugKey));
    }
}

bool KeybindService::SetDebugKey(const unsigned long& acKeyCode, const TiltedPhoques::String& acKeyName) noexcept
{
    m_debugKey = {acKeyName, {m_keyCode, acKeyCode}};
    m_debugService.SetDebugKey(TiltedPhoques::MakeShared<Key>(m_debugKey));

    if (acKeyCode != KeyCodes::Error)
    {
        // DebugService would check if the key was pressed before it gets set here,
        // so this is necessary to toggle ImGui without having to press the key again
        m_debugService.DebugPressed();
    }

    return m_config.SetKey("UI", "sDebugKey", m_debugKey.first.c_str());
}

bool KeybindService::SetUIKey(const unsigned long& acKeyCode, const TiltedPhoques::String& acKeyName) noexcept
{
    m_uiKey = {acKeyName, {m_keyCode, acKeyCode}};
    m_inputService.SetUIKey(TiltedPhoques::MakeShared<Key>(m_uiKey));

    if (m_uiKey.second.diKeyCode != 0)
        m_pInputHook->SetToggleKeys({m_uiKey.second.diKeyCode});

    // InputService would check if the key was pressed before it gets set here,
    // so these are necessary to toggle the UI without having to press the key again
    m_pInputHook->SetEnabled(true);
    m_inputService.Toggle(m_keyCode, MapVirtualKey(m_uiKey.second.vkKeyCode, MAPVK_VK_TO_VSC), KEYEVENT_CHAR);

    return m_config.SetKey("UI", "sUiKey", m_uiKey.first.c_str());
}

bool KeybindService::BindUIKey(const uint16_t& acKeyCode) noexcept
{
    if (acKeyCode == m_debugKey.second.vkKeyCode)
        return false;

    m_uiKeybindConfirmed = false;

    const auto& key = MakeKey(acKeyCode);

    return SetUIKey(KeyCodes::Error, key.first);
}

bool KeybindService::BindDebugKey(const uint16_t& acKeyCode) noexcept
{
    if (acKeyCode == m_uiKey.second.vkKeyCode)
        return false;

    m_debugKeybindConfirmed = false;

    const auto& key = MakeKey(acKeyCode);

    return SetDebugKey(KeyCodes::Error, key.first);
}

void KeybindService::OnVirtualKeyKeyPress(const KeyPressEvent& acEvent) noexcept
{
    m_keyCode = acEvent.VirtualKey;

    if (!m_debugKeybindConfirmed)
    {
        HandleKeybind(KeyCodes::Error);
    }

    if (acEvent.VirtualKey == m_debugKey.second.vkKeyCode && m_debugKeybindConfirmed)
        m_debugService.DebugPressed();
}

void KeybindService::OnDirectInputKeyPress(const unsigned long& acKeyCode) noexcept
{
    if (m_keyCode == 0 || m_keyCode == m_uiKey.second.vkKeyCode)
    {
        // Attempt to reconcile the VirtualKey
        m_keyCode = ReconcileKeyPress();
    }

    // DebugService would sometimes miss debug key's state change so it is handled here
    if (CanToggleDebug(acKeyCode))
    {
        m_debugService.DebugPressed();
    }

    if (m_keyCode != 0 && (!m_uiKeybindConfirmed || !m_debugKeybindConfirmed))
    {
        HandleKeybind(acKeyCode);
    }

    m_keyCode = 0;
}

void KeybindService::HandleKeybind(const unsigned long& acKeyCode) noexcept
{
    TiltedPhoques::String key = {static_cast<char>(toupper(ConvertToUnicode(m_keyCode)))};
    auto modKey = std::ranges::find_if(m_modifiedKeys, [&](const std::pair<Key::first_type, uint16_t>& acKey) { return acKey.second == m_keyCode; });

    // Key has custom name
    if (modKey != m_modifiedKeys.end())
    {
        m_keyCode = modKey->second;

        // UI key pressed
        if (modKey->first == m_uiKey.first && !m_uiKeybindConfirmed)
        {
            key = modKey->first;
            m_uiKeybindConfirmed = true;
            SetUIKey(acKeyCode, key);
        }
        // Debug key pressed
        else if (modKey->first == m_debugKey.first && !m_debugKeybindConfirmed)
        {
            key = modKey->first;
            m_debugKeybindConfirmed = true;
            SetDebugKey(acKeyCode, key);
        }
    }
    // UI key was pressed
    else if (key == m_uiKey.first && !m_uiKeybindConfirmed)
    {
        m_uiKeybindConfirmed = true;
        SetUIKey(acKeyCode, key);
    }
    // Debug key was pressed
    else if (key == m_debugKey.first && !m_debugKeybindConfirmed)
    {
        m_debugKeybindConfirmed = true;
        SetDebugKey(acKeyCode, key);
    }
}

KeybindService::Key KeybindService::MakeKey(const uint16_t& acKeyCode) noexcept
{
    m_keyCode = acKeyCode;

    TiltedPhoques::String newName = {static_cast<char>(toupper(ConvertToUnicode(acKeyCode)))};
    auto modKey = std::ranges::find_if(m_modifiedKeys, [&](const std::pair<Key::first_type, uint16_t>& acKey) { return acKey.second == m_keyCode; });

    if (modKey != m_modifiedKeys.end())
    {
        if (modKey != m_modifiedKeys.end())
        {
            newName = modKey->first;
            m_keyCode = modKey->second;
        }
    }

    return Key{newName, {m_keyCode, KeyCodes::Error}};
}

uint16_t KeybindService::ReconcileKeyPress() noexcept
{
    for (uint16_t key = 255; key > 1; key--)
    {
        if (GetAsyncKeyState(key) & 0x8000)
        {
            return key;
        }
    }

    return 0;
}

wchar_t KeybindService::ConvertToUnicode(const uint16_t& aKeyCode) noexcept
{
    wchar_t buffer[10];
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    ToUnicode(aKeyCode, 0, keyboardState, buffer, 10, 0);

    return buffer[0];
}

bool KeybindService::Config::Create() noexcept
{
    if (this->ini.SaveFile(this->path.c_str(), true) == SI_OK)
    {
        spdlog::info("Successfully created {}", kKeybindsFileName);

        this->ini.SetValue("UI", "sUiKey", "F2");
        this->ini.SetValue("UI", "sDebugKey", "F3");

        if (!this->Save())
        {
            spdlog::warn("Failed to save {}", kKeybindsFileName);
            return false;
        }

        return true;
    }

    spdlog::warn("{} Failed to create {}, using defaults instead", __FUNCTION__, kKeybindsFileName);
    return false;
}

bool KeybindService::Config::Save() const noexcept
{
    return this->ini.SaveFile(this->path.c_str(), true) == SI_OK;
}

bool KeybindService::Config::Load() noexcept
{
    return this->ini.LoadFile(this->path.c_str()) == SI_OK;
}

bool KeybindService::Config::SetKey(const char* acpSection, const char* acpKey, const char* acpValue, const char* acpDescription) noexcept
{
    this->ini.SetValue(acpSection, acpKey, acpValue, acpDescription);

    return this->Save();
}
