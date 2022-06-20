#include "MagicTarget.h"

#include <Actor.h>
#include <Games/ActorExtension.h>
#include <World.h>
#include <Games/ActorExtension.h>
#include "EffectItem.h"
#include "EffectSetting.h"
#include "MagicItem.h"

#include <Events/AddTargetEvent.h>

TP_THIS_FUNCTION(TAddTarget, bool, MagicTarget, MagicTarget::AddTargetData& arData);
TP_THIS_FUNCTION(TCheckAddEffectTargetData, bool, MagicTarget::AddTargetData, void* arArgs, float afResistance);

static TAddTarget* RealAddTarget = nullptr;
static TCheckAddEffectTargetData* RealCheckAddEffectTargetData = nullptr;

static thread_local bool s_autoSucceedEffectCheck = false;

bool MagicTarget::AddTarget(AddTargetData& arData) noexcept
{
    s_autoSucceedEffectCheck = true;
    bool result = ThisCall(RealAddTarget, this, arData);
    s_autoSucceedEffectCheck = false;
    return result;
}

bool TP_MAKE_THISCALL(HookAddTarget, MagicTarget, MagicTarget::AddTargetData& arData)
{
    // TODO: this can be fixed by properly implementing multiple inheritance
    Actor* pTargetActor = (Actor*)((char*)apThis - 0x98);
    ActorExtension* pTargetActorEx = pTargetActor->GetExtension();

    if (!pTargetActorEx)
        return ThisCall(RealAddTarget, apThis, arData);

    if (pTargetActorEx->IsLocalPlayer())
    {
        bool result = ThisCall(RealAddTarget, apThis, arData);
        if (result && arData.pEffectItem->pEffectSetting->eArchetype != EffectArchetypes::ArchetypeID::SUMMON_CREATURE)
            World::Get().GetRunner().Trigger(AddTargetEvent(pTargetActor->formID, arData.pSpell->formID));
        return result;
    }
    else if (pTargetActorEx->IsRemotePlayer())
    {
        return false;
    }

    if (arData.pCaster)
    {
        ActorExtension* pCasterExtension = arData.pCaster->GetExtension();
        if (pCasterExtension->IsLocalPlayer())
        {
            bool result = ThisCall(RealAddTarget, apThis, arData);
            if (result && arData.pEffectItem->pEffectSetting->eArchetype != EffectArchetypes::ArchetypeID::SUMMON_CREATURE)
                World::Get().GetRunner().Trigger(AddTargetEvent(pTargetActor->formID, arData.pSpell->formID));
            return result;
        }
        else if (pCasterExtension->IsRemotePlayer())
        {
            return false;
        }
    }

    if (pTargetActorEx->IsLocal())
    {
        bool result = ThisCall(RealAddTarget, apThis, arData);
        if (result && arData.pEffectItem->pEffectSetting->eArchetype != EffectArchetypes::ArchetypeID::SUMMON_CREATURE)
            World::Get().GetRunner().Trigger(AddTargetEvent(pTargetActor->formID, arData.pSpell->formID));
        return result;
    }
    else
    {
        return false;
    }
}

bool TP_MAKE_THISCALL(HookCheckAddEffectTargetData, MagicTarget::AddTargetData, void* arArgs, float afResistance)
{
    if (s_autoSucceedEffectCheck)
        return true;

    return ThisCall(RealCheckAddEffectTargetData, apThis, arArgs, afResistance);
}

static TiltedPhoques::Initializer s_magicTargetHooks([]() {
    POINTER_FALLOUT4(TAddTarget, addTarget, 385036);
    POINTER_FALLOUT4(TCheckAddEffectTargetData, checkAddEffectTargetData, 113034);

    RealAddTarget = addTarget.Get();
    RealCheckAddEffectTargetData = checkAddEffectTargetData.Get();

    TP_HOOK(&RealAddTarget, HookAddTarget);
    TP_HOOK(&RealCheckAddEffectTargetData, HookCheckAddEffectTargetData);
});

