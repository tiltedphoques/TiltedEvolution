#pragma once

struct MagicCaster
{
    virtual ~MagicCaster();

    enum State : int32_t
    {
        DERIVED_ATTRIBUTE = 0x0,
        ATTRIBUTE = 0x1,
        SKILL = 0x2,
        AI_ATTRIBUTE = 0x3,
        RESISTANCE = 0x4,
        CONDITION = 0x5,
        CHARGE = 0x6,
        INT_VALUE = 0x7,
        VARIABLE = 0x8,
        RESOURCE = 0x9,
        TYPE_COUNT = 0xA,
    };

    GameArray<uint32_t> hSounds;
    int32_t hDesiredTarget;
    MagicItem* pCurrentSpell;
    MagicCaster::State eState;
    float fCastingTimer;
    float fCurrentSpellCost;
    float fMagnitudeOverride;
    float fNextTargetUpdate;
    float fProjectileTimer;
};
