#include "TESContainer.h"

int64_t TESContainer::GetItemCount(TESForm* apItem) const noexcept
{
    TP_THIS_FUNCTION(TGetItemCount, int64_t, const TESContainer);

    POINTER_SKYRIMSE(TGetItemCount, s_getItemCount, 0x14019A3C0 - 0x140000000);

    return ThisCall(s_getItemCount, this);
}

