#include "Services/KeybindService.h"

#include "DInputHook.hpp"
#include "OverlayApp.hpp"

#include "Events/KeyPressEvent.h"

#include "Services/MagicService.h"

KeybindService::KeybindService(entt::dispatcher& aDispatcher, InputService& aInputService, DebugService& aDebugService, MagicService& aMagicService)
    : m_dispatcher(aDispatcher)
    , m_inputService(aInputService)
    , m_debugService(aDebugService)
    , m_magicService(aMagicService)
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

    spdlog::info(L"UI Key: {} | Debug Key: {} | Reveal Players Key: {}", m_uiKey.first.c_str(), m_debugKey.first.c_str(), m_revealPlayersKey.first.c_str());
}

void KeybindService::InitializeKeys(bool aLoadDefaults) noexcept
{
    if (aLoadDefaults)
    {
        m_uiKey = DEFAULT_UI_KEY;
        m_debugKey = DEFAULT_DEBUG_KEY;
        m_revealPlayersKey = DEFAULT_REVEAL_PLAYERS_KEY;
    }
    else
    {
        m_uiKey.first = m_config.ini.GetValue(L"Keybinds", L"sUiKey", L"F2");
        m_debugKey.first = m_config.ini.GetValue(L"Keybinds", L"sDebugKey", L"F3");
        m_revealPlayersKey.first = m_config.ini.GetValue(L"Keybinds", L"sRevealPlayersKey", L"F4");

        CheckForDuplicates();

        m_uiKey.second = m_config.GetKeyCodes(L"ui");
        m_debugKey.second = m_config.GetKeyCodes(L"debug");
        m_revealPlayersKey.second = m_config.GetKeyCodes(L"reveal");
    }

    HandleKeybind(m_uiKey.second.vkKeyCode, m_uiKey.second.diKeyCode, true);
    HandleKeybind(m_debugKey.second.vkKeyCode, m_debugKey.second.diKeyCode, true);
    HandleKeybind(m_revealPlayersKey.second.vkKeyCode, m_revealPlayersKey.second.diKeyCode, true);
}

void KeybindService::CheckForDuplicates() noexcept
{
    // Reset keys based on which are conflicting
    if (m_uiKey.first == m_debugKey.first)
    {
        spdlog::warn("UI and Debug keys are the same, resetting Debug key");
        m_debugKey = DEFAULT_DEBUG_KEY;
    }

    if (m_uiKey.first == m_revealPlayersKey.first)
    {
        spdlog::warn("UI and Reveal Players keys are the same, resetting Reveal Players key");
        m_revealPlayersKey = DEFAULT_REVEAL_PLAYERS_KEY;
    }

    if (m_debugKey.first == m_revealPlayersKey.first)
    {
        spdlog::warn("Debug and Reveal Players keys are the same, resetting Reveal Players key");
        m_revealPlayersKey = DEFAULT_REVEAL_PLAYERS_KEY;
    }
}

const KeybindService::Key& KeybindService::GetKey(const Keybind& acKeyType) const noexcept
{
    switch (acKeyType)
    {
    default: break;
    case None: break;
    case UI: return m_uiKey;
    case Debug: return m_debugKey;
    case RevealPlayers: return m_revealPlayersKey;
    }

    return Key{L"", {KeyCodes::Error, KeyCodes::Error}};
}

bool KeybindService::SetKey(const Keybind& acKeyType, const Key& acKey, const bool& acLoadFromConfig) noexcept
{
    switch (acKeyType)
    {
    default:
    case None:
        spdlog::warn("{} Invalid key type", __FUNCTION__);
        return false; 
    case UI:
        return HandleSetUI(acKey, acLoadFromConfig);
    case Debug:
        return HandleSetDebug(acKey, acLoadFromConfig);
    case RevealPlayers:
        return HandleSetRevealPlayers(acKey, acLoadFromConfig);
    }
}

bool KeybindService::SetKeyValues(Key& acKeyToChange, const Key& acKey) noexcept
{
    if (acKeyToChange.first == acKey.first)
    {
        if (acKey.second.vkKeyCode != KeyCodes::Error)
            acKeyToChange.second.vkKeyCode = acKey.second.vkKeyCode;

        if (acKey.second.diKeyCode != KeyCodes::Error)
            acKeyToChange.second.diKeyCode = acKey.second.diKeyCode;

        // sanity check
        if (!acKey.first.empty())
            acKeyToChange.first = acKey.first;

        return true;
    }

    return false;
}

bool KeybindService::HandleSetUI(const Key& acKey, const bool& acLoadFromConfig) noexcept
{
    if (SetKeyValues(m_uiKey, acKey))
    {
        m_inputService.SetUIKey(TiltedPhoques::MakeShared<Key>(m_uiKey));
        m_pInputHook->SetToggleKeys({m_uiKey.second.diKeyCode});

        m_uiKeybindConfirmed =
            m_uiKey.second.vkKeyCode != KeyCodes::Error && m_uiKey.second.diKeyCode != KeyCodes::Error;
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
        m_inputService.Toggle(m_uiKey.second.vkKeyCode, MapVirtualKey(m_uiKey.second.vkKeyCode, MAPVK_VK_TO_VSC),
                              KEYEVENT_CHAR);

        return m_config.SetKey(L"sUiKey", m_uiKey.first.c_str()) && m_config.SetKeyCodes(L"ui", m_uiKey.second);
    }

    return true;
}

bool KeybindService::HandleSetDebug(const Key& acKey, const bool& acLoadFromConfig) noexcept
{
    if (SetKeyValues(m_debugKey, acKey))
    {
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

bool KeybindService::HandleSetRevealPlayers(const Key& acKey, const bool& acLoadFromConfig) noexcept
{
    if (SetKeyValues(m_revealPlayersKey, acKey))
    {
        // do reveal player stuff here?
    }
    else
    {
        // Keybind may have been changed via config file, start reconciling the key
        m_revealPlayersKey.second = {KeyCodes::Error, KeyCodes::Error};
        m_revealPlayersKeybindConfirmed = false;
    }

    if (!acLoadFromConfig)
        return m_config.SetKey(L"sRevealPlayersKey", m_revealPlayersKey.first.c_str()) && m_config.SetKeyCodes(L"reveal", m_revealPlayersKey.second);

    return true;
}

bool KeybindService::BindKey(const Keybind& acKeyType, const uint16_t& acNewKeyCode) noexcept
{
    if (acNewKeyCode == m_debugKey.second.vkKeyCode || acNewKeyCode == m_uiKey.second.vkKeyCode ||
        acNewKeyCode == m_revealPlayersKeybindConfirmed || acKeyType == Keybind::None)
        return false;

    const auto& boundKey = HandleBind(acKeyType, acNewKeyCode);

    switch (acKeyType)
    {
    case UI:
        m_uiKeybindConfirmed = !boundKey;
        break;
    case Debug:
        m_debugKeybindConfirmed = !boundKey;
        break;
    case RevealPlayers:
        m_revealPlayersKeybindConfirmed = !boundKey;
        break;
    default: break;
    }

    return boundKey;
}

bool KeybindService::HandleBind(const Keybind& acKeyType, const uint16_t& acNewKeyCode) noexcept
{
    const auto& cKey = MakeKey(acNewKeyCode);

    switch (acKeyType)
    {
    case None:
        break;
    case UI:
        m_uiKey = cKey;
        break;
    case Debug:
        m_debugKey = cKey;
        break;
    case RevealPlayers:
        m_revealPlayersKey = cKey;
        break;
    }

    return SetKey(acKeyType, cKey);
}

void KeybindService::OnVirtualKeyKeyPress(const KeyPressEvent& acEvent) noexcept
{
    m_keyCode = acEvent.VirtualKey >= 0x10 && acEvent.VirtualKey <= 0x12 ? ResolveVkKeyModifier(acEvent.VirtualKey) : acEvent.VirtualKey;

    if (!m_debugKeybindConfirmed || !m_uiKeybindConfirmed || !m_revealPlayersKeybindConfirmed)
    {
        HandleKeybind(m_keyCode, KeyCodes::Error);
    }

    if (CanToggleDebug(m_keyCode, KeyCodes::Error))
    {
        m_debugService.DebugPressed();
    }
    else if (CanRevealOtherPlayers(m_keyCode, KeyCodes::Error))
    {
        m_magicService.RevealKeybindPressed();
        //const auto& canRevealOtherPlayers = m_magicService.GetCanRevealOtherPlayers();
        //m_magicService.RevealOtherPlayers(!canRevealOtherPlayers);
    }
}

void KeybindService::OnDirectInputKeyPress(const unsigned long& acKeyCode) noexcept
{
    if (!m_uiKeybindConfirmed || !m_debugKeybindConfirmed || !m_revealPlayersKeybindConfirmed)
    {
        HandleKeybind(KeyCodes::Error, acKeyCode);
    }

    // DebugService would sometimes miss debug key's state change so it is handled here
    if (CanToggleDebug(KeyCodes::Error, acKeyCode))
    {
        m_debugService.DebugPressed();
    }
    else if (CanRevealOtherPlayers(KeyCodes::Error, acKeyCode))
    {
        m_magicService.RevealKeybindPressed();
        //const auto& canRevealOtherPlayers = m_magicService.GetCanRevealOtherPlayers();
        //m_magicService.RevealOtherPlayers(!canRevealOtherPlayers);
    }
}

void KeybindService::HandleKeybind(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode, const bool& acLoadFromConfig) noexcept
{
    m_keyCode = acVkKeyCode;

    // Attempt to reconcile VirtualKey
    if (m_keyCode == KeyCodes::Error && !acLoadFromConfig)
    {
        // TODO (Toe Knee): this can still miss keystate changes but is the lesser of two evils...
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
            SetKey(Keybind::UI, Key{pModKey->first, {m_keyCode, acDiKeyCode}}, acLoadFromConfig);
        }
        // Debug key pressed
        else if (DoKeysMatch(*pModKey, m_debugKey) && !m_debugKeybindConfirmed)
        {
            SetKey(Keybind::Debug, Key{pModKey->first, {m_keyCode, acDiKeyCode}});
        }
    }
    // No custom key name
    else
    {
        if (DoKeysMatch(cKey, m_uiKey) && !m_uiKeybindConfirmed)
        {
            SetKey(Keybind::UI, Key{cKeyName, {m_keyCode, acDiKeyCode}}, acLoadFromConfig);
        }
        else if (DoKeysMatch(cKey, m_debugKey) && !m_debugKeybindConfirmed)
        {
            SetKey(Keybind::Debug, Key{cKeyName, {m_keyCode, acDiKeyCode}});
        }
    }
}

KeybindService::Key KeybindService::MakeKey(const uint16_t& acVkKeyCode) noexcept
{
    m_keyCode = acVkKeyCode;

    TiltedPhoques::WString newName = {static_cast<wchar_t>(MapVirtualKeyW(acVkKeyCode, MAPVK_VK_TO_CHAR))};
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
    return !m_isTextInputFocused && ((acVkKeyCode != KeyCodes::Error && acVkKeyCode == m_debugKey.second.vkKeyCode) || 
        (acDiKeyCode != KeyCodes::Error && acDiKeyCode == m_debugKey.second.diKeyCode));
}

bool KeybindService::CanRevealOtherPlayers(const uint16_t& acVkKeyCode, const unsigned long& acDiKeyCode) const noexcept
{
    return !m_isTextInputFocused && ((acVkKeyCode != KeyCodes::Error && acVkKeyCode == m_revealPlayersKey.second.vkKeyCode) ||
                                     (acDiKeyCode != KeyCodes::Error && acDiKeyCode == m_revealPlayersKey.second.diKeyCode));
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

    this->ini.SetValue(L"Keybinds", L"sUiKey", L"F2");
    this->ini.SetValue(L"Keybinds", L"sDebugKey", L"F3");
    this->ini.SetValue(L"Keybinds", L"sRevealPlayersKey", L"F4");

    const TiltedPhoques::WString cUiKeyCodes{std::to_wstring(VK_F2) + L"," + std::to_wstring(DIK_F2)};
    const TiltedPhoques::WString cDebugKeyCodes{std::to_wstring(VK_F3) + L"," + std::to_wstring(DIK_F3)};
    const TiltedPhoques::WString cRevealPlayersKeyCodes{std::to_wstring(VK_F4) + L"," + std::to_wstring(DIK_F4)};

    this->ini.SetValue(L"Internal", L"ui", cUiKeyCodes.c_str());
    this->ini.SetValue(L"Internal", L"debug", cDebugKeyCodes.c_str());
    this->ini.SetValue(L"Internal", L"reveal", cRevealPlayersKeyCodes.c_str());

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
    this->ini.SetValue(L"Keybinds", acpKey, acpValue, acpDescription);

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
