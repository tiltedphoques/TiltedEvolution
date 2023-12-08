#pragma once

#include "Actor.h"

struct PlayerCharacter : Actor
{
    static PlayerCharacter* Get()
    {
        return (PlayerCharacter*)0x145594D28; // 865059
    }

    virtual void sub_1A2();
    virtual void sub_1A3();
    virtual void sub_1A4();
    virtual void sub_1A5();

    uint8_t unkPlayerCharacter[0xEF8 - sizeof(Actor)];
    TESForm* pCurrentLocation;
    uint8_t unkF00[0x1100 - 0xF00];
};

static_assert(sizeof(PlayerCharacter) == 0x1100);