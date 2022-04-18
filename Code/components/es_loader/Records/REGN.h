#pragma once

#include "Chunks.h"
#include "Record.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/REGN
// REGN are "regions", but not in the locational sense (those are LCTN). Each region ties together ambient information
// (weather/music/etc), presumably so they can be dropped into settings and provide a complete ambience.
class REGN : public Record
{
  public:
    static constexpr FormEnum kType = FormEnum::REGN;
    // EDID
    String m_editorId = "";

    // RDWT_ID 
    std::vector<Chunks::RDWT> m_weatherTypes{};

    void ParseChunks(REGN& aSourceRecord, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
};
