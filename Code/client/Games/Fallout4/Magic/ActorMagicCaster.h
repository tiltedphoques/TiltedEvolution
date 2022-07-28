#pragma once

#include "MagicCaster.h"

struct ActorMagicCaster : MagicCaster
{
    uint8_t pad48[0xB8 - 0x48];
    Actor* pCasterActor;
    uint8_t padC0[0x100 - 0xC0];
};

static_assert(offsetof(ActorMagicCaster, pad48) == 0x48);
static_assert(sizeof(ActorMagicCaster) == 0x100);
