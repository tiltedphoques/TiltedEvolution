#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/TES4
class TES4 : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::CONT;

    // Master files
    Vector<Chunks::MAST> m_masterFiles{};

    TES4 ParseChunks() noexcept;
};
