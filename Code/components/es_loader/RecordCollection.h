#pragma once

#include "Records/CLMT.h"
#include "Records/CONT.h"
#include "Records/GMST.h"
#include "Records/NAVM.h"
#include "Records/NPC.h"
#include "Records/REFR.h"
#include "Records/REGN.h"
#include "Records/WRLD.h"

namespace ESLoader
{
class RecordCollection
{
    friend class TESFile;

  public:
    template <typename T> using RecordSet = TiltedPhoques::Map<uint32_t, T>;

    FormEnum GetFormType(uint32_t aFormId) const noexcept
    {
        auto record = m_allRecords.find(aFormId);
        if (record == std::end(m_allRecords))
        {
            spdlog::error("Record not found for form id {:X}", aFormId);
            return FormEnum::EMPTY_ID;
        }

        return record->second.GetType();
    }

    bool HasAnyRecords() const noexcept
    {
        return m_allRecords.size();
    }

    REFR& GetObjectRefById(uint32_t aFormId) noexcept
    {
        return m_objectReferences[aFormId];
    }
    CLMT& GetClimateById(uint32_t aFormId) noexcept
    {
        return m_climates[aFormId];
    }
    NPC& GetNpcById(uint32_t aFormId) noexcept
    {
        return m_npcs[aFormId];
    }
    CONT& GetContainerById(uint32_t aFormId) noexcept
    {
        return m_containers[aFormId];
    }
    GMST& GetGameSettingById(uint32_t aFormId) noexcept
    {
        return m_gameSettings[aFormId];
    }
    WRLD& GetWorldById(uint32_t aFormId) noexcept
    {
        return m_worlds[aFormId];
    }
    NAVM& GetNavMeshById(uint32_t aFormId) noexcept
    {
        return m_navMeshes[aFormId];
    }

    const RecordSet<CLMT>& GetClimates() noexcept
    {
        return m_climates;
    }

    const RecordSet<REGN>& GetRegions() noexcept
    {
        return m_regions;
    }

    void BuildReferences();

  private:
    RecordSet<Record> m_allRecords{};
    RecordSet<REFR> m_objectReferences{};
    RecordSet<CLMT> m_climates{};
    RecordSet<NPC> m_npcs{};
    RecordSet<CONT> m_containers{};
    RecordSet<GMST> m_gameSettings{};
    RecordSet<WRLD> m_worlds{};
    RecordSet<NAVM> m_navMeshes{};
    RecordSet<REGN> m_regions{};
};

} // namespace ESLoader
