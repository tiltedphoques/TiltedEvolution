#pragma once

#include "Record.h"
#include "Chunks.h"

class REFR : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::REFR;

    Chunks::NAME m_basicObject{};
    Chunks::MapMarkerData m_markerData{};

    void ParseChunks(REFR& aSourceRecord, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;
};

