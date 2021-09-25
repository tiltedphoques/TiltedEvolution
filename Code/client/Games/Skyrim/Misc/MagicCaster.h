#pragma once

#include "MagicSystem.h"

struct ActiveEffect;
struct MagicItem;

struct MagicCaster
{
    virtual ~MagicCaster();
    virtual uint64_t CastSpellImmediate(MagicItem* apSpell, bool abLoadCast, TESObjectREFR* apDesiredTarget,
                                       float afEffectivenessMult, bool abAdjustOnlyHostileEffectiveness,
                                       float afMagnitudeOverride);
    virtual uint64_t FindTouchTarget();
    virtual char RequestCastImpl();
    virtual bool StartChargeImpl();
    virtual char StartReadyImpl();
    virtual char StartCastImpl();
    virtual void FinishCastImpl();
    virtual uint64_t InterruptCastImpl(bool abRefund);
    virtual uint64_t SpellCast(bool abSuccess, uint32_t auiTargetCount, MagicItem* apSpell);
    virtual uint64_t CheckCast(MagicItem* apSpell, bool abDualCast, float* afEffectStrength,
                              MagicSystem::CannotCastReason* apReason, bool abUseBaseMagicka);
    virtual uint64_t GetCasterStatsObject();
    virtual uint64_t GetCasterAsActor();
    virtual uint64_t GetCasterObjectReference(Actor** appCasterActor);
    virtual uint64_t GetMagicNode();
    virtual void ClearMagicNode();
    virtual void SetCurrentSpellImpl(MagicItem* apSpell);
    virtual uint64_t SelectSpellImpl();
    virtual uint64_t DeselectSpellImpl();
    virtual void SetSkipCheckCast();
    virtual void SetCastingTimerForCharge();
    virtual uint64_t GetCastingSource();
    virtual uint64_t GetIsDualCasting();
    virtual void SetDualCasting(bool abValue);
    virtual void* SaveGame(BGSSaveFormBuffer* apSaveGameBuffer);
    virtual uint64_t LoadGame(BGSLoadFormBuffer* apLoadGameBuffer);
    virtual uint64_t FinishLoadGame(BGSLoadFormBuffer* apLoadGameBuffer);
    virtual void PrepareSound(MagicSystem::SoundID aeID, MagicItem* apSpell);
    virtual uint64_t AdjustActiveEffect(ActiveEffect* apActiveEffect, float afEffectStrength,
                                       bool abAdjustOnlyHostileEffectiveness);

    enum State
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

    GameArray<uint64_t> hSounds; // BSTArray<BSSoundHandle> hSounds;
    int32_t hDesiredTarget;      // BSPointerHandle<TESObjectREFR,BSUntypedPointerHandle<21,5> > hDesiredTarget;
    MagicItem* pCurrentSpell;
    MagicCaster::State eState;
    float fCastingTimer;
    float fCurrentSpellCost;
    float fMagnitudeOverride;
    float fNextTargetUpdate;
    float fProjectileTimer;
};

static_assert(sizeof(MagicCaster) == 0x48);

