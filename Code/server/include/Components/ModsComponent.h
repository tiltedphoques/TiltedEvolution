#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct ModsComponent
{
    struct Entry
    {
        uint32_t id;
        uint32_t refCount;
    };

    uint32_t AddStandard(const String& acpFilename) noexcept;
    uint32_t AddLite(const String& acpFilename) noexcept;

    const auto& GetStandardMods() const noexcept { return m_standardMods; }
    const auto& GetLiteMods() const noexcept { return m_liteMods; }

    using TModList = Map<String, Entry>; 
private:

    uint32_t m_seed = 0;
    Map<String, Entry> m_standardMods;
    Map<String, Entry> m_liteMods;
};
