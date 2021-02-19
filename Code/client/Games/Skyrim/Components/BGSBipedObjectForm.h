#pragma once

#include <Components/BaseFormComponent.h>

struct BGSBipedObjectForm : BaseFormComponent
{
    enum class Part
    {
        Head = 1 << 0,
        Hair = 1 << 1,
        Body = 1 << 2,
        Hands = 1 << 3,
        Forearms = 1 << 4,
        Amulet = 1 << 5,
        Ring = 1 << 6,
        Feet = 1 << 7,
        Calves = 1 << 8,
        Shield = 1 << 9,
        LongHair = 1 << 11,
        Circlet = 1 << 12,
        Ears = 1 << 13
    };

    uint32_t bitfield;
    uint32_t unk8;
};

