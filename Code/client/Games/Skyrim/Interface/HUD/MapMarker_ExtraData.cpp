
#include <Games/Skyrim/Interface/HUD/MapMarker_ExtraData.h>

MapMarkerData* MapMarkerData::New() noexcept
{
    MapMarkerData* pMarkerData = Memory::Allocate<MapMarkerData>();

    // MapMarker_14014ace0_ExtraData
    TP_THIS_FUNCTION(TMapMarkerDataCtor, void, MapMarkerData);
    POINTER_SKYRIMSE(TMapMarkerDataCtor, s_mapMarkerDataCtor, 12945);
    ThisCall(s_mapMarkerDataCtor, pMarkerData);

    return pMarkerData;
}
