#pragma once

struct CombatInventory
{
    uint8_t unk0[0x1B8];
    float maximumRange;
    uint8_t unk1BC[0x1C8 - 0x1BC];
};
static_assert(sizeof(CombatInventory) == 0x1C8);
