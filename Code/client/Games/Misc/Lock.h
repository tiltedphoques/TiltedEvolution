#pragma once

struct Lock
{

    enum class Flag : uint8_t
    {
        kNone = 0,
        kLocked = 1 << 0,
        kLeveled = 1 << 2
    };

    void SetLock(bool aIsLocked) noexcept;
    bool IsLocked() const noexcept
    {
        return (flags & static_cast<uint8_t>(Flag::kLocked)) != 0;
    }

    uint8_t lockLevel;
    uint8_t pad1[7];
    void* unk8;
    uint8_t flags;
    uint8_t pad11[3];
    uint32_t numberOfTries;
};

static_assert(sizeof(Lock) == 0x18);
