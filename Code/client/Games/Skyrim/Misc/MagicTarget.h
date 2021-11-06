#pragma once

#include <Misc/MagicSystem.h>

struct Actor;
struct MagicItem;
struct EffectSetting;
struct TESBoundObject;

struct MagicTarget
{
    struct IPostCreationModification;
    struct ResultsCollector;

    struct AddTargetData
    {
        bool CheckAddEffect(void* arArgs, float afResistance);

        Actor* pCaster;
        MagicItem* pSpell;
        EffectSetting* pEffectSetting;
        TESBoundObject* pSource;
        IPostCreationModification* pCallback;
        ResultsCollector* pResultsCollector;
        NiPoint3 ExplosionLocation;
        float fMagnitude;
        float fUnkFloat1; // seems to always be 1.0?
        MagicSystem::CastingSource eCastingSource;
        bool bAreaTarget;
        bool bDualCast;
    };

    virtual ~MagicTarget();

    bool AddTarget(AddTargetData& arData) noexcept;
    // this function actually adds the effect
    bool CheckAddEffect(AddTargetData& arData) noexcept;

    void* unk04;
    void* unk08;
};
//constexpr size_t t = offsetof(MagicTarget::AddTargetData, eCastingSource);
