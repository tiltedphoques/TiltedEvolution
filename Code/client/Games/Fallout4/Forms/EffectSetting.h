#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>
#include <Components/BGSKeywordForm.h>
#include <Components/BGSMenuDisplayObject.h>
#include <Games/Magic/MagicSystem.h>

struct EffectSetting : TESForm
{
    TESFullName fullName;
    BGSMenuDisplayObject menuDisplayObject;
    BGSKeywordForm keywordForm;
    void* filterValidationFunction;
    void* filterValidationItem;
    uint32_t flags;
    uint8_t pad74[0xD0 - 0x74];
    EffectArchetypes::ArchetypeID eArchetype;
    uint8_t padD4[0xF0 - 0xD4];
    uint32_t castType;
    uint32_t deliveryType;
    uint8_t padF8[0x1B0 - 0xF8];
};

static_assert(offsetof(EffectSetting, flags) == 0x70);
static_assert(offsetof(EffectSetting, eArchetype) == 0xD0);
static_assert(offsetof(EffectSetting, castType) == 0xF0);
static_assert(sizeof(EffectSetting) == 0x1B0);
