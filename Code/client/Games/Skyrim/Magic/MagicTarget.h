#pragma once

#include <Games/Magic/MagicSystem.h>

struct Actor;
struct MagicItem;
struct EffectItem;
struct TESBoundObject;
struct ActiveEffect;

struct MagicTarget
{
    struct IPostCreationModification;
    struct ResultsCollector;

    struct ForEachActiveEffectVisitor
    {
        virtual ~ForEachActiveEffectVisitor();

        virtual bool Visit(ActiveEffect* apEffect) = 0;
    };
    static_assert(sizeof(ForEachActiveEffectVisitor) == 0x8);

    // this struct is a lot simpler in Fallout 4, it just passes a ref to AddTargetData
    struct ResetElapsedTimeMatchingEffects : ForEachActiveEffectVisitor
    {
        MagicItem* pSpell;
        Actor* pCaster;
        EffectItem* pEffectItem;
        TESBoundObject* pSource;
        MagicSystem::CastingSource eCastingSource;
        bool bResetOne;
    };
    static_assert(sizeof(ResetElapsedTimeMatchingEffects) == 0x30);

    struct HasSameUsageEffect : ForEachActiveEffectVisitor
    {
        EffectItem* pUsage;
        ActiveEffect* spFoundEffect;
    };
    static_assert(sizeof(HasSameUsageEffect) == 0x18);

    struct AddTargetData
    {
        bool CheckAddEffect(void* arArgs, float afResistance);

        bool ShouldSync();

        Actor* pCaster;
        MagicItem* pSpell;
        EffectItem* pEffectItem;
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

    Actor* GetTargetAsActor();

    bool AddTarget(AddTargetData& arData) noexcept;
    // this function actually adds the effect
    bool CheckAddEffect(AddTargetData& arData) noexcept;
    void DispelAllSpells(bool aNow) noexcept;

    void* unk04;
    void* unk08;
};
