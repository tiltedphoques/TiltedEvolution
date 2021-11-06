#include "MagicTarget.h"

#include <Actor.h>
#include <Games/ActorExtension.h>
#include <World.h>
#include <Games/ActorExtension.h>

#include <Events/AddTargetEvent.h>

TP_THIS_FUNCTION(TAddTarget, bool, MagicTarget, MagicTarget::AddTargetData& arData);
TP_THIS_FUNCTION(TCheckAddEffect, bool, MagicTarget::AddTargetData, void* arArgs, float afResistance);

static TAddTarget* RealAddTarget = nullptr;
static TCheckAddEffect* RealCheckAddEffect = nullptr;

static thread_local bool s_autoSucceedEffectCheck = false;

bool MagicTarget::AddTarget(AddTargetData& arData) noexcept
{
    s_autoSucceedEffectCheck = true;
    bool result = ThisCall(RealAddTarget, this, arData);
    s_autoSucceedEffectCheck = false;
    return result;
}

/* TODO:
* Don't send continuous, value modifying effects like burning from flames concentration spell.
    Can't cancel all ValueModifierEffects though.

* The conjuration visuals script effect is not being applied because, while we don't
    cancel that one, only the summon effect, the form id component is not created yet
    at the time of dispatching this event.
*/
bool TP_MAKE_THISCALL(HookAddTarget, MagicTarget, MagicTarget::AddTargetData& arData)
{
    // TODO: this can be fixed by properly implementing multiple inheritance
    Actor* pTargetActor = (Actor*)((char*)apThis - 0x98);
    spdlog::info("pTargetActor: {:X}", pTargetActor->formID);
    ActorExtension* pTargetActorEx = pTargetActor->GetExtension();

    bool triggerLocal = false;

    auto factions = pTargetActor->GetFactions();
    for (const auto& faction : factions.NpcFactions)
    {
        if (faction.Id.BaseId == 0x00000DB1)
        {
            if (pTargetActorEx->IsRemote())
                return false;

            triggerLocal = true;
            break;
        }
    }

    if (arData.pCaster && !triggerLocal)
    {
        if (auto pExtension = arData.pCaster->GetExtension())
        {
            if (pExtension->IsRemote())
                return false;
        }
    }

    bool result = ThisCall(RealAddTarget, apThis, arData);
    if (result && arData.pEffectItem->pEffectSetting->eArchetype != EffectArchetypes::ArchetypeID::SUMMON_CREATURE)
        World::Get().GetRunner().Trigger(AddTargetEvent(pTargetActor->formID, arData.pSpell->formID));
    return result;
}

bool TP_MAKE_THISCALL(HookCheckAddEffect, MagicTarget::AddTargetData, void* arArgs, float afResistance)
{
    if (s_autoSucceedEffectCheck)
        return true;

    return ThisCall(RealCheckAddEffect, apThis, arArgs, afResistance);
}

static TiltedPhoques::Initializer s_magicTargetHooks([]() {
    POINTER_SKYRIMSE(TAddTarget, addTarget, 0x140633090 - 0x140000000);
    POINTER_SKYRIMSE(TCheckAddEffect, checkAddEffect, 0x1405535C0 - 0x140000000);

    RealAddTarget = addTarget.Get();
    RealCheckAddEffect = checkAddEffect.Get();

    TP_HOOK(&RealAddTarget, HookAddTarget);
    TP_HOOK(&RealCheckAddEffect, HookCheckAddEffect);
});

