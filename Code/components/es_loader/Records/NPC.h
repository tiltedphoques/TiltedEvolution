#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/NPC
class NPC : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::NPC_;

    Chunks::ACBS m_baseStats{};
    Chunks::DOFT m_defaultOutfit{};

    NPC ParseChunks() noexcept;
};
