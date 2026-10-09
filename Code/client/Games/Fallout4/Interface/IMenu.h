#pragma once

#include <Misc/BSFixedString.h>

struct IMenu
{
    uint8_t pad0[0x50];
    BSFixedString menuName;
    uint32_t uiMenuFlags;
    uint8_t pad5C[0x14];
};

static_assert(offsetof(IMenu, menuName) == 0x50);
static_assert(offsetof(IMenu, uiMenuFlags) == 0x58);
static_assert(sizeof(IMenu) == 0x70);
