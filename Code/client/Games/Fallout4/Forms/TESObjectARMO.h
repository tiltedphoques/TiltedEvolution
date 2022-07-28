#pragma once

#include "TESBoundObject.h"
#include <Misc/TBO_InstanceData.h>

struct BGSTypedFormValuePair
{
    union SharedVal {
        unsigned int i;
        float f;
    };
};

enum class STAGGER_MAGNITUDE : int32_t
{
    STAGGER_NONE = 0x0,
    STAGGER_SMALL = 0x1,
    STAGGER_MEDIUM = 0x2,
    STAGGER_LARGE = 0x3,
    STAGGER_EXTRA_LARGE = 0x4,
    STAGGER_MAGNITUDE_COUNT = 0x5,
    STAGGER_MAGNITUDE_MIN = 0x0,
    STAGGER_MAGNITUDE_MAX = 0x4,
};

struct TESObjectARMO : TESBoundObject, TESFullName
{
    struct InstanceData : TBO_InstanceData
    {
        GameArray<EnchantmentItem*>* pEnchantments;
        GameArray<BGSMaterialSwap*>* pMaterialSwapA;
        BGSBlockBashData* pBlockBashData;
        BGSKeywordForm* pKeywords;
        GameArray<std::tuple<TESForm*, BGSTypedFormValuePair::SharedVal>>* pDamageTypesA;
        GameArray<std::tuple<TESForm*, BGSTypedFormValuePair::SharedVal>>* pActorValuesA;
        float fWeight;
        float fColorRemappingIndex;
        uint32_t uiValue;
        uint32_t uiHealth;
        STAGGER_MAGNITUDE eStaggerRating;
        uint16_t usRating;
        uint16_t usIndex;
    };
};
