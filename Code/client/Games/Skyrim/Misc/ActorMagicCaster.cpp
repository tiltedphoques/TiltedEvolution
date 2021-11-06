#include "ActorMagicCaster.h"

#include <Events/SpellCastEvent.h>
#include <Events/InterruptCastEvent.h>

#include <Games/Skyrim/Actor.h>
#include <Games/ActorExtension.h>

#include <World.h>

TP_THIS_FUNCTION(TSpellCast, void, ActorMagicCaster, bool sbSuccess, int32_t auiTargetCount, MagicItem* apSpell);
TP_THIS_FUNCTION(TInterruptCast, void, ActorMagicCaster, bool);

static TSpellCast* RealSpellCast = nullptr;
static TInterruptCast* RealInterruptCast = nullptr;

void TP_MAKE_THISCALL(HookSpellCast, ActorMagicCaster, bool abSuccess, int32_t auiTargetCount, MagicItem* apSpell)
{
    if (!abSuccess)
        return;

    auto* pActor = apThis->pCasterActor;

    if (pActor->GetExtension()->IsRemote())
        return;

    //World::Get().GetRunner().Trigger(SpellCastEvent(apThis, apSpell));

    spdlog::info("HookSpellCast, abSuccess: {}, auiTargetCount: {}, apSpell: {:X}", abSuccess, auiTargetCount, (uint64_t)apSpell);

    ThisCall(RealSpellCast, apThis, abSuccess, auiTargetCount, apSpell);
}

void TP_MAKE_THISCALL(HookInterruptCast, ActorMagicCaster, bool abRefund)
{
    auto* pExtended = apThis->pCasterActor->GetExtension();

    if (pExtended->IsLocal())
    {
        spdlog::info("Send Interrupt cast event");
        World::Get().GetRunner().Trigger(InterruptCastEvent(apThis->pCasterActor->formID));
    }

    ThisCall(RealInterruptCast, apThis, abRefund);
}

static TiltedPhoques::Initializer s_actorMagicCasterHooks([]() {
    POINTER_SKYRIMSE(TSpellCast, s_spellCast, 0x140542470 - 0x140000000);
    POINTER_SKYRIMSE(TInterruptCast, s_interruptCast, 0x140541D90 - 0x140000000);

    RealSpellCast = s_spellCast.Get();
    RealInterruptCast = s_interruptCast.Get();

    TP_HOOK(&RealSpellCast, HookSpellCast);
    TP_HOOK(&RealInterruptCast, HookInterruptCast);
});
