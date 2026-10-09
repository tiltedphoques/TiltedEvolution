#include <Forms/TESWorldSpace.h>

TESObjectCELL* TESWorldSpace::LoadCell(int32_t aXCoordinate, int32_t aYCoordinate) noexcept
{
    TP_THIS_FUNCTION(TLoadCell, TESObjectCELL*, TESWorldSpace, int16_t, int16_t);
    static VersionDbPtr<TLoadCell> loadCell(2202852);
    return TiltedPhoques::ThisCall(loadCell, this, static_cast<int16_t>(aXCoordinate), static_cast<int16_t>(aYCoordinate));
}
