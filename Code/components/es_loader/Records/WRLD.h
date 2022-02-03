#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/WRLD
class WRLD : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::WRLD;

    String m_editorId = "";
    std::optional<Chunks::WCTR> m_centerCell;
    std::optional<uint32_t> m_climateId;
    std::optional<Chunks::DNAM> m_landData;
    std::optional<uint32_t> m_parentId;
    uint32_t m_musicId;
    float m_lodMultiplier;

    void ParseChunks(WRLD& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
};
