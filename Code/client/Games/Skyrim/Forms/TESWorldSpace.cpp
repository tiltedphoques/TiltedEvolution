#include "TESWorldSpace.h"

TESObjectCELL* TESWorldSpace::LoadCell(int32_t aXCoordinate, int32_t aYCoordinate) noexcept
{
    TP_THIS_FUNCTION(TLoadCell, TESObjectCELL*, TESWorldSpace, int32_t aXCoordinate, int32_t aYCoordinate);
    POINTER_SKYRIMSE(TLoadCell, s_loadCell, 20460);
    return TiltedPhoques::ThisCall(s_loadCell, this, aXCoordinate, aYCoordinate);
}
