#pragma once

#include <Games/ExtraDataList.h>

struct BGSEncounterZone;

struct ExtraEncounterZone : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::EncounterZone;

    BGSEncounterZone* pEncounterZone;
};
