#pragma once

#include "Records/CLMT.h"
#include "Records/CONT.h"
#include "Records/NPC.h"
#include "Records/REFR.h"
#include "Records/GMST.h"
#include "Records/NAVM.h"
#include "Records/WRLD.h"

class RecordCollection
{
    friend class TESFile;

public:
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
    TiltedPhoques::Map<uint32_t, Record> m_allRecords{};
    TiltedPhoques::Map<uint32_t, REFR> m_objectReferences{};
    TiltedPhoques::Map<uint32_t, CLMT> m_climates{};
    TiltedPhoques::Map<uint32_t, NPC> m_npcs{};
    TiltedPhoques::Map<uint32_t, CONT> m_containers{};
    TiltedPhoques::Map<uint32_t, GMST> m_gameSettings{};
    TiltedPhoques::Map<uint32_t, WRLD> m_worlds{};
    TiltedPhoques::Map<uint32_t, NAVM> m_navMeshes{};
};