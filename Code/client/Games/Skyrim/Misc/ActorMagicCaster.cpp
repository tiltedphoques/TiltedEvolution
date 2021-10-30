#include "ActorMagicCaster.h"

#include <Events/SpellCastEvent.h>

#include <Games/Skyrim/Actor.h>
#include <Games/ActorExtension.h>

#include <World.h>

TP_THIS_FUNCTION(TSpellCast, void, ActorMagicCaster, bool sbSuccess, int32_t auiTargetCount, MagicItem* apSpell);

static TSpellCast* RealSpellCast = nullptr;

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

static TiltedPhoques::Initializer s_actorMagicCasterHooks([]() {
    POINTER_SKYRIMSE(TSpellCast, s_spellCast, 0x140542470 - 0x140000000);

    RealSpellCast = s_spellCast.Get();

    TP_HOOK(&RealSpellCast, HookSpellCast);
});
