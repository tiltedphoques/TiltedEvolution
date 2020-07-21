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

    const Map<String, Entry>& GetStandardMods() const noexcept { return m_standardMods; }
    const Map<String, Entry>& GetLiteMods() const noexcept { return m_liteMods; }

private:

    uint32_t m_seed = 0;
    Map<String, Entry> m_standardMods;
    Map<String, Entry> m_liteMods;
};
