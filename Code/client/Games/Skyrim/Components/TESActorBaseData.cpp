#include <TiltedOnlinePCH.h>

#include <Components/TESActorBaseData.h>
#include <Forms/TESActorBase.h>

// Kept for reference: Actor::GetLeveledPick reads the engine's ExtraLeveledCreature directly.
// Skyrim takes a single template base here; Fallout 4 takes a template array.

using TCreateTemplateActorBase = TESActorBase* __fastcall(TESActorBase*, TESActorBase*);
static TCreateTemplateActorBase* RealCreateTemplateActorBase = nullptr;

// In Skyrim, called by ExtraDataList::LoadGame (12095) and some unknown function (23277)
TESActorBase* HookCreateTemplateActorBase(TESActorBase* apOriginalBase, TESActorBase* apTemplateBase)
{
    TESActorBase* pResult = RealCreateTemplateActorBase(apOriginalBase, apTemplateBase);

    spdlog::debug(
        "Leveled resolution: original base {:X} -> template base {:X}, temp base {:X}", apOriginalBase ? apOriginalBase->formID : 0, apTemplateBase ? apTemplateBase->formID : 0,
        pResult ? pResult->formID : 0);

    return pResult;
}

static TiltedPhoques::Initializer s_actorBaseDataInitHooks(
    []()
    {
        // POINTER_SKYRIMSE(TCreateTemplateActorBase, s_CreateTemplateActorBase, 14375);
        // RealCreateTemplateActorBase = s_CreateTemplateActorBase.Get();
        // TP_HOOK(&RealCreateTemplateActorBase, HookCreateTemplateActorBase);
    });
