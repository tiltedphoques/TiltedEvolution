#include "TESContainer.h"

int64_t TESContainer::GetItemCount(TESForm* apItem) const noexcept
{
    TP_THIS_FUNCTION(TGetItemCount, int64_t, const TESContainer);

    POINTER_SKYRIMSE(TGetItemCount, s_getItemCount, 14529);

    return TiltedPhoques::ThisCall(s_getItemCount, this);
}

