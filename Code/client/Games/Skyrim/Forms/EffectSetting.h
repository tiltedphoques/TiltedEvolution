#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>
#include <Components/BGSKeywordForm.h>
#include <Components/BGSMenuDisplayObject.h>
#include <Games/Magic/MagicSystem.h>


struct EffectSetting : TESForm
{
    TESFullName    fullName;
    BGSMenuDisplayObject menuDisplayObject;
    BGSKeywordForm keywordForm;
    uintptr_t unk30;
    uintptr_t unk34;
    uint32_t flags;
    uint32_t unk3C;
    void* unk40;
    uint32_t unk44;
    uint32_t unk48;
    uint16_t unk4C;
    uint8_t pad4E[2];
    void* unk50;
    uint32_t unk54;
    void* unk58;
    void* unk5C;
    uint8_t unk60[0x18];
    EffectArchetypes::ArchetypeID eArchetype;
    void* unk80[2];
    uint32_t castType;
    uint32_t deliveryType;
    // more stuff
};

static_assert(offsetof(EffectSetting, eArchetype) == 0xC0);
