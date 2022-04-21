#include "TESWorldSpace.h"

TESObjectCELL* TESWorldSpace::LoadCell(int32_t aX, int32_t aY) noexcept
{
    TP_THIS_FUNCTION(TLoadCell, TESObjectCELL*, TESWorldSpace, int32_t aX, int32_t aY);
    POINTER_SKYRIMSE(TLoadCell, s_loadCell, 20460);
    return ThisCall(s_loadCell, this, aX, aY);
}
