#pragma once

#include "ActiveEffect.h"

#include <Actor.h>
#include <Games/ActorExtension.h>

struct SummonCreatureEffect : ActiveEffect
{
};

TP_THIS_FUNCTION(TInitializeSummonCreatureEffect, void*, Actor, MagicItem*, EffectItem*);
static TInitializeSummonCreatureEffect* RealInitializeSummonCreatureEffect = nullptr;

void* TP_MAKE_THISCALL(HookInitializeSummonCreatureEffect, Actor, MagicItem* apSpell, EffectItem* apEffect)
{
    if (apThis)
    {
        auto pExtension = apThis->GetExtension();
        if (pExtension && pExtension->IsRemote())
            return nullptr;
    }

    return ThisCall(RealInitializeSummonCreatureEffect, apThis, apSpell, apEffect);
}

static TiltedPhoques::Initializer s_summonCreatureEffectsHooks([]()
{
    POINTER_SKYRIMSE(TInitializeSummonCreatureEffect, s_start, 0x1405636B0 - 0x140000000);

    RealInitializeSummonCreatureEffect = s_start.Get();

    TP_HOOK(&RealInitializeSummonCreatureEffect, HookInitializeSummonCreatureEffect);
});

