#include "Services/RebindService.h"

RebindService::RebindService(InputService& aInputService) :
    m_inputService(aInputService)
{
}

std::shared_ptr<RebindService::Key> RebindService::GetKeyFromName(const TiltedPhoques::String& acKeyName) const noexcept
{
    if (const auto& key = std::ranges::find_if(m_keys, [acKeyName](const Key& aKey) { return aKey.first == acKeyName; }); key != m_keys.end())
    {
        return std::make_shared<Key>(*key);
    }

    return nullptr;
}

std::shared_ptr<RebindService::Key> RebindService::GetKeyFromVKKeyCode(int32_t aKeyCode) const noexcept
{
    // Toe Knee: small chance it might not find the key in time before checking for it?
    if (const auto& key = std::ranges::find_if(m_keys, [aKeyCode](const Key& aKey) { return aKey.second.vkKeyCode == aKeyCode; }); key != m_keys.end())
    {
        return std::make_shared<Key>(*key);
    }

    return nullptr;
}

std::shared_ptr<RebindService::Key> RebindService::GetKeyFromDIKeyCode(int32_t aKeyCode) const noexcept
{

    if (const auto& key = std::ranges::find_if(m_keys, [aKeyCode](const Key& aKey) { return aKey.second.diKeyCode == aKeyCode; }); key != m_keys.end())
    {
        return std::make_shared<Key>(*key);
    }

    return nullptr;
}

int32_t RebindService::GetDIKeyCode(const TiltedPhoques::String& aKeyName) const noexcept
{
    for (const auto& key : m_keys)
    {
        if (key.first == aKeyName)
            return key.second.diKeyCode;
    }

    return KeyCodes::Error;
}

int32_t RebindService::GetVKKeyCode(const TiltedPhoques::String& aKeyName) const noexcept
{
    for (const auto& key : m_keys)
    {
        if (key.first == aKeyName)
            return key.second.vkKeyCode;
    }

    return KeyCodes::Error;
}

bool RebindService::SetUIKey(std::shared_ptr<RebindService::Key> apKey) noexcept
{
    m_pUiKey = *apKey;
    m_inputService.SetUIKey(std::move(apKey));
    return true;
}

bool RebindService::SetUIKeyFromVKKeyCode(int32_t aKeyCode) noexcept
{
    const auto& key = GetKeyFromVKKeyCode(aKeyCode);

    if (!key->first.empty())
    {
        m_pUiKey = *key;
        m_inputService.SetUIKey(std::move(key));
        return true;
    }

    return false;
}

bool RebindService::SetUIKeyFromDIKeyCode(int32_t aKeyCode) noexcept
{
    const auto& key = GetKeyFromDIKeyCode(aKeyCode);

    if (!key->first.empty())
    {
        m_pUiKey = *key;
        m_inputService.SetUIKey(key);
        return true;
    }

    return false;
}

bool RebindService::SetUIKeyFromKeyName(const TiltedPhoques::String& acKeyName) noexcept
{
    if (const auto& key = GetKeyFromName(acKeyName); !key->first.empty())
    {
        m_pUiKey = *key;
        m_inputService.SetUIKey(key);
        return true;
    }

    return false;
}

