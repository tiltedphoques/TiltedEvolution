#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/CLMT
class GMST : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::GMST;

    // EDID
    String m_editorId = "";
    // DATA
    Chunks::TypedValue m_value{};

    void ParseChunks(GMST& aSourceRecord, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
};
