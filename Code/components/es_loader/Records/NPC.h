#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/NPC
class NPC : Record
{
public:
    static constexpr FormEnum kType = FormEnum::NPC_;

    struct Data
    {
        Chunks::ACBS m_baseStats{};
        Chunks::DOFT m_defaultOutfit{};
    };

    Data ParseChunks() noexcept;
};
