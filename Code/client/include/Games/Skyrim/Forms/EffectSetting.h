#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/Components/TESFullName.h>
#include <Games/Skyrim/Components/BGSKeywordForm.h>
#include <Games/Skyrim/Components/BGSMenuDisplayObject.h>


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
    uint32_t unk60[8];
    void* unk80[2];
    uint32_t castType;
    uint32_t deliveryType;
    // more stuff
};

#endif
