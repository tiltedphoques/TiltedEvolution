#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Actor.h>

struct PlayerCharacter : Actor
{
    static PlayerCharacter* Get() noexcept;

    uint8_t padPlayerCharacter[0xCC8 - sizeof(Actor)];
    TESForm *locationForm;
    uint8_t pad_CD0[0x140];
};

static_assert(sizeof(PlayerCharacter) == 0xE10);
static_assert(offsetof(PlayerCharacter, locationForm) == 0xCC8);

#endif
