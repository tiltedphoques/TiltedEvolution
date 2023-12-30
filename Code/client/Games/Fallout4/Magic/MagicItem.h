#pragma once

#include <Forms/TESBoundObject.h>
#include <Components/TESFullName.h>
#include <Components/BGSKeywordForm.h>

struct EffectItem;
struct EffectSetting;

struct MagicItem : TESBoundObject, TESFullName, BGSKeywordForm
{
    // TODO: ft
    EffectItem* GetEffect(const uint32_t aEffectId) noexcept
    {
        return nullptr;
    }

    GameArray<EffectItem*> listOfEffects;
    int32_t iHostileCount;
    EffectSetting* pAVEffectSetting;
    uint32_t uiPreloadCount;
    void* spPreloadedItem;
};
