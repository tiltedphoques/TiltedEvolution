#pragma once

#include <ExtraData/ExtraData.h>
#include <Interface/HUD/MapMarker_ExtraData.h>

struct ExtraMapMarker : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::MapMarker;

    MapMarkerData* pMarkerData = nullptr;
};

static_assert(sizeof(ExtraMapMarker) == 0x18);

