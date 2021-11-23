#pragma once

#include <Forms/TESForm.h>
#include <Games/Magic/MagicSystem.h>

struct EffectSetting : TESForm
{
    uint8_t pad20[0xD0 - 0x20];
    EffectArchetypes::ArchetypeID eArchetype;
    uint8_t padD4[0x1B0 - 0xD4];
};

static_assert(sizeof(EffectSetting) == 0x1B0);

