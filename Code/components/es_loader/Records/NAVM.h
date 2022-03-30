#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/NAVM
class NAVM : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::NAVM;

    Chunks::NVNM m_navMesh;

    void ParseChunks(NAVM& aSourceRecord, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
};
