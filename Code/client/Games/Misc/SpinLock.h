#pragma once

struct SpinLock
{
    void Lock() noexcept;
    void Unlock() noexcept;

    uint32_t parentThread;
    uint32_t lockCount;
};
