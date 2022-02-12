#include <stdafx.h>

#include <Components.h>

uint32_t ModsComponent::AddStandard(const String& acpFilename) noexcept
{
    const auto itor = m_standardMods.find(acpFilename);
    if (itor != std::end(m_standardMods))
    {
        itor.value().refCount++;
        return itor->second.id;
    }

    const auto id = m_seed++;
    m_standardMods.emplace(acpFilename, Entry{id, 1});
    m_globalList.push_back(acpFilename);

    return id;
}

uint32_t ModsComponent::AddLite(const String& acpFilename) noexcept
{
    const auto itor = m_liteMods.find(acpFilename);
    if (itor != std::end(m_liteMods))
    {
        itor.value().refCount++;
        return itor->second.id;
    }

    const auto id = m_seed++;
    m_liteMods.emplace(acpFilename, Entry{id, 1});
    m_globalList.push_back(acpFilename);

    return id;
}

bool ModsComponent::IsKnown(const String& acpFilename)
{
    auto it =
        std::find_if(m_globalList.begin(), m_globalList.end(), [&](const String& aStr) { return aStr == acpFilename; });

    return it != m_globalList.end();
}
