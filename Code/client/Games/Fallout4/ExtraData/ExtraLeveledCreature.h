#pragma once

#include "ExtraData.h"

struct TESNPC;

struct ExtraLeveledCreature : BSExtraData
{
    virtual ~ExtraLeveledCreature();

    uint8_t unk20[0x68]; // This buffer is copied like a pod
};

static_assert(sizeof(ExtraLeveledCreature) == 0x88);
static_assert(offsetof(ExtraLeveledCreature, unk20) == 0x20);
