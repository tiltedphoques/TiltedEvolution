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
