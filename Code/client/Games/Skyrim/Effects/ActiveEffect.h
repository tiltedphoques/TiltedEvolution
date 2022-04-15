#pragma once

#include <Games/Magic/MagicSystem.h>

struct MagicTarget;
struct EffectItem;

struct ActiveEffect
{
    // this ctor is used by seemingly all ActiveEffect child classes' Instantiate() methods
    // can be used to find individual Instantiate() methods
    // address: 0x140C4E350
    ActiveEffect(Actor* apCaster, MagicItem* apSpell, EffectItem* apEffect);

    virtual void sub_0();
    virtual void sub_1();
    virtual void sub_2();
    virtual void sub_3();
    virtual void sub_4();
    virtual void sub_5();
    virtual void sub_6();
    virtual void sub_7();
    virtual void sub_8();
    virtual void sub_9();
    virtual void sub_A();
    virtual void sub_B();
    virtual void sub_C();
    virtual void sub_D();
    virtual void sub_E();
    virtual void sub_F();
    virtual void sub_10();
    virtual void sub_11();
    virtual void sub_12();
    virtual void sub_13();
    virtual void Start();
    virtual void sub_15();
    virtual void sub_16();
    virtual void sub_17();
    virtual void sub_18();

    static ActiveEffect* Instantiate(Actor* apCaster, MagicItem* apSpell, EffectItem* apEffect);

    uint8_t pad8[0x38];
    MagicItem* pSpell;
    void* pEffect;
    MagicTarget* pTarget;
    TESBoundObject* pSource;
    void* pHitEffects;
    MagicItem* pDisplacementSpell;
    float fElapsedSeconds;
    float fDuration;
    float fMagnitude;
    uint32_t uiFlags;
    uint32_t eConditionStatus;
    uint16_t usUniqueID;
    MagicSystem::CastingSource eCastingSource;
};
static_assert(sizeof(ActiveEffect) == 0x90);

namespace ActiveEffectFactory
{
    ActiveEffect* Activate(Actor* apCaster, MagicSystem::CastingSource aeCastingSource, MagicItem* apSpell,
                           EffectItem* apEffectItem, TESBoundObject* apSource, bool abWornEnchantment);
};

