

#include "BSCore/BSSpinLock.h"

void BSSpinLock::Lock() noexcept
{
    const uint32_t threadId = ::GetCurrentThreadId();
    if (OwningThread == threadId)
    {
        uiLockCount++;
        return;
    }

    uint32_t attempts = 0;

    uint32_t ref = 0;
    while (uiLockCount.compare_exchange_weak(ref, 1u, std::memory_order_acquire))
    {
        uint32_t sleepCount = 0;

        if (attempts >= kFastSpinThreshold)
        {
            sleepCount = 1;
        }
        else
        {
            ++attempts;
            sleepCount = 0;
        }

        ::Sleep(sleepCount);
    }

    OwningThread = threadId;
}

void BSSpinLock::Unlock() noexcept
{
    const uint32_t threadId = ::GetCurrentThreadId();

    if (OwningThread == threadId)
    {
        if (uiLockCount == 1)
        {
            OwningThread = 0;
            uiLockCount = 0;
        }
        else
        {
            uiLockCount--;
        }
    }
}
