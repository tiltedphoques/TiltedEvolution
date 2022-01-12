#pragma once

struct EffectSetting;

struct EffectItemData
{
    float fMagnitude;
    int32_t iArea;
    int32_t iDuration;
};

struct EffectItem
{
    EffectItemData data;
    EffectSetting* pEffectSetting;
    float fRawCost;
    void* Conditions;
};

