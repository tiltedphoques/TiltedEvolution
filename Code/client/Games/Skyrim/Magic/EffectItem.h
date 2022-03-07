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
    bool IsHealingEffect() noexcept;

    EffectItemData data;
    uint32_t padC;
    EffectSetting* pEffectSetting;
    float fRawCost;
};
