#pragma once

class BSSpinLock
{
  public:
    static constexpr uint32_t kFastSpinThreshold = 10000;

    BSSpinLock() = default;

    void Lock() noexcept;
    void Unlock() noexcept;

  private:
    // members
    uint32_t OwningThread = 0;
    std::atomic<std::uint32_t> uiLockCount = 0;
};
static_assert(sizeof(BSSpinLock) == 8);

// utility class defined by us
class BSScopedSpinLock
{
  public:
    explicit BSScopedSpinLock(BSSpinLock& aLock) : m_lock(aLock)
    {
        aLock.Lock();
    }

    ~BSScopedSpinLock()
    {
        m_lock.Unlock();
    }

  private:
    BSSpinLock& m_lock;
};
