#pragma once

#include <Games/ExtraData.h>
#include <Interface/HUD/MapMarker_ExtraData.h>

struct ExtraMapMarker : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::MapMarker;

    MapMarkerData* pMarkerData = nullptr;
};

static_assert(sizeof(ExtraMapMarker) == 0x18);

