#pragma once


#include <Forms/TESBoundObject.h>
#include <Forms/EffectSetting.h>
#include <Misc/EffectItem.h>
#include <Components/BGSKeywordForm.h>
#include <Components/TESFullName.h>

struct MagicItem : TESBoundObject
{
    TESFullName fullName;
    BGSKeywordForm keyword;
    GameArray<EffectItem*> listOfEffects;
    int32_t iHostileCount;
    EffectSetting* pAVEffectSetting;
    uint32_t unk48;
    uint32_t unk4C;
};
