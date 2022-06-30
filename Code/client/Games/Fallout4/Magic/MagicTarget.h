#pragma once

#include <Effects/ActiveEffect.h>
#include <Games/Magic/MagicSystem.h>

struct Actor;
struct EffectItem;
struct MagicItem;
struct TESBoundObject;

struct MagicTarget
{
    struct IPostCreationModification;
    struct ResultsCollector;

    struct AddTargetData
    {
        Actor* pCaster;
        MagicItem* pSpell;
        EffectItem* pEffectItem;
        TESBoundObject* pSource;
        MagicTarget::IPostCreationModification* pCallback;
        MagicTarget::ResultsCollector* pResultsCollector;
        NiPoint3 ExplosionLocation;
        float fMagnitude;
        MagicSystem::CastingSource eCastingSource;
        bool bAreaTarget;
        bool bDualCast;
    };

    struct SpellDispelData
    {
        const MagicItem* pSpell;
        int32_t hCaster;
        GamePtr<ActiveEffect> spActiveEffect;
        SpellDispelData* pNext;
    };

    enum Flag : int32_t
    {
        MTF_UPDATING = 0x1,
        MTF_INVISIBLE = 0x2,
    };

    virtual ~MagicTarget();

    Actor* GetTargetAsActor();

    bool AddTarget(AddTargetData& arData) noexcept;
    // this function actually adds the effect
    bool CheckAddEffect(AddTargetData& arData) noexcept;
    void DispelAllSpells(bool aNow) noexcept;

    SpellDispelData* pPostUpdateDispelList;
    Flag ucFlags;
};
