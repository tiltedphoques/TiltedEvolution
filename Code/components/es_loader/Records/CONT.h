#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/CONT
class CONT : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::CONT;

    // EDID
    String m_editorId = "";
    // FULL
    String m_name = "";
    // Objects
    Vector<Chunks::CNTO> m_objects{};

    void ParseChunks(CONT& aSourceRecord, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
};
