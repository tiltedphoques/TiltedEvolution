#pragma once

// Ported from CommonLibSSE (RE/B/BSAtomic.h). Field and function names are the engine's own.

struct BSCriticalSection
{
    CRITICAL_SECTION CriticalSection;
};

static_assert(sizeof(BSCriticalSection) == 0x28);

// Recursive, exclusive lock. The engine inlines it, so this mirrors its code.
struct BSSpinLock
{
    enum : uint32_t
    {
        kFastSpinThreshold = 10000,
    };

    void Lock() noexcept;
    void Unlock() noexcept;

    volatile uint32_t OwningThread{0};
    volatile uint32_t uiLockCount{0};
};

static_assert(sizeof(BSSpinLock) == 0x8);

// Many readers or one (recursive) writer. Always lock through the engine's functions; a different
// algorithm on the same memory can break the lock for every thread (see ExtraDataList).
struct BSReadWriteLock
{
    void LockForRead() noexcept;
    void UnlockRead() noexcept;
    void LockForWrite() noexcept;
    void UnlockWrite() noexcept;

    volatile uint32_t uiWriterThread{0};
    volatile uint32_t uiLock{0};
};

static_assert(sizeof(BSReadWriteLock) == 0x8);

struct BSSpinLockGuard
{
    [[nodiscard]] explicit BSSpinLockGuard(BSSpinLock& aLock) noexcept
        : m_lock(aLock)
    {
        m_lock.Lock();
    }
    ~BSSpinLockGuard() noexcept { m_lock.Unlock(); }

    BSSpinLockGuard(const BSSpinLockGuard&) = delete;
    BSSpinLockGuard& operator=(const BSSpinLockGuard&) = delete;

private:
    BSSpinLock& m_lock;
};

struct BSWriteLockGuard
{
    [[nodiscard]] explicit BSWriteLockGuard(BSReadWriteLock& aLock) noexcept
        : m_lock(aLock)
    {
        m_lock.LockForWrite();
    }
    ~BSWriteLockGuard() noexcept { m_lock.UnlockWrite(); }

    BSWriteLockGuard(const BSWriteLockGuard&) = delete;
    BSWriteLockGuard& operator=(const BSWriteLockGuard&) = delete;

private:
    BSReadWriteLock& m_lock;
};
