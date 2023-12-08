#include "TESFormRefCount.h"

#include <windows.h>

uint64_t TESFormRefCount::IncRefCount() noexcept
{
    // TODO: preferably use in game func as well
    InterlockedIncrement(&refcount);
    return refcount;
}

uint64_t TESFormRefCount::DecRefCount() noexcept
{
    using TDecRefCount = uint64_t(TESFormRefCount*);
    TDecRefCount* decRefCount = (TDecRefCount*)0x1407599a0; // 35164
    return decRefCount(this);
}
