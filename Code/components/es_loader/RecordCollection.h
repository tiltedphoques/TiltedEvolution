#pragma once

#include "Records/CLMT.h"
#include "Records/CONT.h"
#include "Records/GMST.h"
#include "Records/NAVM.h"
#include "Records/NPC.h"
#include "Records/REFR.h"
#include "Records/WRLD.h"

namespace ESLoader
{
struct RecordCollection
{
    friend class TESFile;

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

    void BuildReferences();

private:
    Map<uint32_t, Record> m_allRecords{};
    Map<uint32_t, REFR> m_objectReferences{};
    Map<uint32_t, CLMT> m_climates{};
    Map<uint32_t, NPC> m_npcs{};
    Map<uint32_t, CONT> m_containers{};
    Map<uint32_t, GMST> m_gameSettings{};
    Map<uint32_t, WRLD> m_worlds{};
    Map<uint32_t, NAVM> m_navMeshes{};
};

} // namespace ESLoader
