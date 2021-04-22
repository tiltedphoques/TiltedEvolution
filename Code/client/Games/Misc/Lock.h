#pragma once

struct Lock
{
    void SetLock(bool abIsLocked) noexcept;

    uint8_t lockLevel;
    uint8_t pad1[7];
    void* unk8;
    uint8_t flags;
    uint8_t pad11[3];
    uint32_t numberOfTries;
};

static_assert(sizeof(Lock) == 0x18);
