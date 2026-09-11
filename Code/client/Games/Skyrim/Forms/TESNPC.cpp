#include <TiltedOnlinePCH.h>

#include <Forms/TESNPC.h>

using TCreateTemplateActorBase = TESActorBase* __fastcall(TESActorBase*, TESActorBase**);
static TCreateTemplateActorBase* RealCreateTemplateActorBase = nullptr;

// Cache the first template base, which named leveled NPCs can hide from the template chain.
// Temporary form IDs are recycled and cell attach bypasses this hook, so entries can be stale.
// Consumers must prefer the template chain and use this cache only as a last resort.
// The lock protects loader-thread writes against game-thread reads.
static std::mutex s_leveledPicksLock;
static TiltedPhoques::Map<uint32_t, uint32_t> s_leveledPicks;

TESActorBase* HookCreateTemplateActorBase(TESActorBase* apOriginalBase, TESActorBase** appTemplateBaseA)
{
    TESActorBase* pTemplateBase = appTemplateBaseA ? appTemplateBaseA[0] : nullptr;
    TESActorBase* pResult = RealCreateTemplateActorBase(apOriginalBase, appTemplateBaseA);

    spdlog::debug("Leveled resolution: original base {:X} -> template base {:X}, temp base {:X}", apOriginalBase ? apOriginalBase->formID : 0, pTemplateBase ? pTemplateBase->formID : 0, pResult ? pResult->formID : 0);

    if (pResult && pTemplateBase && pResult->formType == FormType::Npc && pTemplateBase->formType == FormType::Npc)
    {
        std::lock_guard lock(s_leveledPicksLock);
        s_leveledPicks[pResult->formID] = pTemplateBase->formID;
    }

    return pResult;
}

uint32_t TESNPC::GetLeveledPickFormId(uint32_t aTempNpcFormId) noexcept
{
    std::lock_guard lock(s_leveledPicksLock);

    const auto cIt = s_leveledPicks.find(aTempNpcFormId);
    return cIt != s_leveledPicks.end() ? cIt->second : 0;
}

static TiltedPhoques::Initializer s_npcInitHooks(
    []()
    {
        POINTER_SKYRIMSE(TCreateTemplateActorBase, s_CreateTemplateActorBase, 14375);

        RealCreateTemplateActorBase = s_CreateTemplateActorBase.Get();

        TP_HOOK(&RealCreateTemplateActorBase, HookCreateTemplateActorBase);
    });
