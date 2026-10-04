#pragma once

#include <Games/Primitives.h>

struct ActiveEffect;
struct MagicItem;
struct EffectItem;
struct TESBoundObject;
struct Actor;
struct TESObjectREFR;
struct AddTargetData;
struct SpellDispelData;
struct ActiveEffectList;

// Fallout 4 MagicTarget: 0x18. Virtuals 0x00-0x0C.
struct MagicTarget
{
    virtual ~MagicTarget();

    virtual bool AddTarget(AddTargetData& arData);
    virtual TESObjectREFR* GetTargetStatsObject();
    virtual bool MagicTargetIsActor();
    virtual bool IsInvulnerable() const;
    virtual void InvalidateCommandedActorEffect(ActiveEffect* apEffect);
    virtual bool CanAddActiveEffect() const;
    virtual ActiveEffectList* GetActiveEffectList();
    virtual float CheckResistance(MagicItem* apSpell, EffectItem* apEffect, TESBoundObject* apSource) const;
    virtual void EffectAdded(ActiveEffect* apEffect);
    virtual void EffectRemoved(ActiveEffect* apEffect);
    virtual void EffectActiveStatusChanged(ActiveEffect* apEffect);
    virtual bool CheckAbsorb(Actor* apCaster, MagicItem* apSpell, const EffectItem* apEffectItem);

    SpellDispelData* postUpdateDispelList; // 0x08
    int8_t flags;                          // 0x10
};

static_assert(sizeof(MagicTarget) == 0x18);
