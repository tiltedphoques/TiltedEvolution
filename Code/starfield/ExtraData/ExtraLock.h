#pragma once

// TODO: account for leveled locks? numTries?

struct REFR_LOCK
{
    bool IsLocked() const noexcept { return flags & 1; }

    uint8_t unk0[0x8];
    uint32_t numTries;
    uint8_t flags;
    uint8_t baseLevel;
};
