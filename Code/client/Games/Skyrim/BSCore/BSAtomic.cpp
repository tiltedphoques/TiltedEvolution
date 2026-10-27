#include "BSAtomic.h"

void BSSpinLock::Lock() noexcept
{
    const uint32_t cThreadId = GetCurrentThreadId();

    _mm_lfence();
    if (OwningThread == cThreadId)
    {
        InterlockedIncrement(&uiLockCount);
    }
    else
    {
        if (InterlockedCompareExchange(&uiLockCount, 1, 0))
        {
            uint32_t spinCount = 0;
            while (InterlockedCompareExchange(&uiLockCount, 1, 0))
                Sleep(++spinCount < kFastSpinThreshold ? 0 : 1);

            _mm_lfence();
        }

        OwningThread = cThreadId;
        _mm_sfence();
    }
}

void BSSpinLock::Unlock() noexcept
{
    const uint32_t cThreadId = GetCurrentThreadId();

    _mm_lfence();
    if (OwningThread == cThreadId)
    {
        if (uiLockCount == 1)
        {
            OwningThread = 0;
            _mm_mfence();
            InterlockedCompareExchange(&uiLockCount, 0, 1);
        }
        else
        {
            InterlockedDecrement(&uiLockCount);
        }
    }
}

void BSReadWriteLock::LockForRead() noexcept
{
    TP_THIS_FUNCTION(TLockForRead, void, BSReadWriteLock);
    POINTER_SKYRIMSE(TLockForRead, s_lockForRead, 68233);
    TiltedPhoques::ThisCall(s_lockForRead, this);
}

void BSReadWriteLock::UnlockRead() noexcept
{
    TP_THIS_FUNCTION(TUnlockRead, void, BSReadWriteLock);
    POINTER_SKYRIMSE(TUnlockRead, s_unlockRead, 68239);
    TiltedPhoques::ThisCall(s_unlockRead, this);
}

void BSReadWriteLock::LockForWrite() noexcept
{
    TP_THIS_FUNCTION(TLockForWrite, void, BSReadWriteLock);
    POINTER_SKYRIMSE(TLockForWrite, s_lockForWrite, 68234);
    TiltedPhoques::ThisCall(s_lockForWrite, this);
}

void BSReadWriteLock::UnlockWrite() noexcept
{
    TP_THIS_FUNCTION(TUnlockWrite, void, BSReadWriteLock);
    POINTER_SKYRIMSE(TUnlockWrite, s_unlockWrite, 68240);
    TiltedPhoques::ThisCall(s_unlockWrite, this);
}
