#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/NPC
class NPC : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::NPC_;

    String m_editorId = "";
    Chunks::ACBS m_baseStats{};
    Chunks::DOFT m_defaultOutfit{};
    Chunks::VMAD m_scriptData{};

    void ParseChunks(NPC& aSourceRecord, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
};
