#pragma once


#include <Forms/TESBoundObject.h>
#include <Components/BGSKeywordForm.h>
#include <Components/TESFullName.h>

struct EffectSetting;

struct MagicItem : TESBoundObject
{
    TESFullName fullName;
    BGSKeywordForm keyword;
    GameArray<void*> unk34;
    uint32_t unk40;
    EffectSetting* unk44;
    uint32_t unk48;
    uint32_t unk4C;
};
