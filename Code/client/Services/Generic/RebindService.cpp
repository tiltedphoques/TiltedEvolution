#include "Services/RebindService.h"

RebindService::RebindService(InputService& aInputService) :
    m_inputService(aInputService)
{
}

std::shared_ptr<RebindService::Key> RebindService::GetKeyFromName(const TiltedPhoques::String& acKeyName) const noexcept
{
    const auto& key = std::find_if(m_keys.begin(), m_keys.end(), [acKeyName](const Key& aKey) { return aKey.first == acKeyName; });

    std::shared_ptr newKey = std::make_shared<Key>(*key);

    if (key != m_keys.end())
    {
        *newKey = *key;
    }

    return newKey;
}

std::shared_ptr<RebindService::Key> RebindService::GetKeyFromVKKeyCode(int32_t aKeyCode) const noexcept
{
    const auto& key = std::find_if(m_keys.begin(), m_keys.end(), [aKeyCode](const Key& aKey) { return aKey.second.vkKeyCode == aKeyCode; });

    std::shared_ptr newKey = std::make_shared<Key>(*key);

    if (key != m_keys.end())
    {
        *newKey = *key;
    }

    return newKey;
}

std::shared_ptr<RebindService::Key> RebindService::GetKeyFromDIKeyCode(int32_t aKeyCode) const noexcept
{
    const auto& key = std::find_if(m_keys.begin(), m_keys.end(), [aKeyCode](const Key& aKey) { return aKey.second.diKeyCode == aKeyCode; });

    std::shared_ptr newKey = std::make_shared<Key>(*key);

    if (key != m_keys.end())
    {
        *newKey = *key;
    }

    return newKey;
}

int32_t RebindService::GetDIKeyCode(const TiltedPhoques::String& aKeyName) const noexcept
{
    for (const auto& key : m_keys)
    {
        if (key.first == aKeyName)
            return key.second.diKeyCode;
    }

    /*if (const auto& key = m_keys.find(aKeyName); key != m_keys.end())
    {
        return m_keys.find(aKeyName)->second.diKeyCode;
    }*/

    return KeyCodes::Error;
}

int32_t RebindService::GetVKKeyCode(const TiltedPhoques::String& aKeyName) const noexcept
{
    for (const auto& key : m_keys)
    {
        if (key.first == aKeyName)
            return key.second.vkKeyCode;
    }

    /*if (const auto& key = m_keys.find(aKeyName); key != m_keys.end())
    {
        return m_keys.find(aKeyName)->second.vkKeyCode;
    }*/

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

