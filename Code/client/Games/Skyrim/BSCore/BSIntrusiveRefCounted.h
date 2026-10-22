#pragma once

struct BSIntrusiveRefCounted
{
    uint32_t IncRef() const noexcept { return InterlockedIncrement(&refCount); }
    uint32_t DecRef() const noexcept { return InterlockedDecrement(&refCount); }

    mutable volatile LONG refCount;
};

static_assert(sizeof(BSIntrusiveRefCounted) == 0x4);
