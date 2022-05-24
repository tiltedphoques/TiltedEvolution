#pragma once

#include <Games/Magic/MagicSystem.h>

struct ActiveEffect;
struct MagicItem;
struct TESObjectREFR;
struct Actor;
struct BGSSaveFormBuffer;
struct BGSLoadFormBuffer;
struct Projectile;

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
    virtual MagicSystem::CastingSource GetCastingSource();
    virtual bool GetIsDualCasting();
    virtual void SetDualCasting(bool abValue);
    virtual void* SaveGame(BGSSaveFormBuffer* apSaveGameBuffer);
    virtual uint64_t LoadGame(BGSLoadFormBuffer* apLoadGameBuffer);
    virtual uint64_t FinishLoadGame(BGSLoadFormBuffer* apLoadGameBuffer);
    virtual void PrepareSound(MagicSystem::SoundID aeID, MagicItem* apSpell);
    virtual uint64_t AdjustActiveEffect(ActiveEffect* apActiveEffect, float afEffectStrength,
                                       bool abAdjustOnlyHostileEffectiveness);

    bool CastSpell(MagicItem* apSpell, TESObjectREFR* apDesiredTarget, bool abLoadCast) noexcept;
    void InterruptCast() noexcept;
    void FindTargets(const NiPoint3& arLocation, Projectile* apProjectile, TESObjectREFR* apCollidee,
                     float afPowerScale, float afAreaOverride, bool abAreaOverrideInWorldUnits, bool abApplyAll);

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
    BSPointerHandle<TESObjectREFR*> hDesiredTarget;
    MagicItem* pCurrentSpell;
    MagicCaster::State eState;
    float fCastingTimer;
    float fCurrentSpellCost;
    float fMagnitudeOverride;
    float fNextTargetUpdate;
    float fProjectileTimer;
};

static_assert(sizeof(MagicCaster) == 0x48);

