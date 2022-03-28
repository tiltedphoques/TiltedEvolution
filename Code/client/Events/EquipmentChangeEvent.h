#pragma once

struct EquipmentChangeEvent
{
    uint32_t ActorId{ 0 };
    bool Unequip = false;
    bool IsLeft = false;
    bool IsSpell = false;
    bool IsShout = false;
};
