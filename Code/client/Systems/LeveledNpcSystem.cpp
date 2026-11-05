#include <TiltedOnlinePCH.h>

#include <Systems/LeveledNpcSystem.h>
#include <Actor.h>
#include <ExtraData/ExtraLeveledCreature.h>
#include <Forms/TESNPC.h>
#if defined(TP_FALLOUT4)
#include <Components/TESActorBaseData.h>
#include <Misc/DeferredDeleter.h>
#else
#include <Misc/GarbageCollector.h>
#endif

#if defined(TP_FALLOUT4)
namespace
{
constexpr uint32_t kTemplateSlotCount = 13;
}
#endif

bool LeveledNpcSystem::IsLeveledNpcBase(const TESNPC* apBase) noexcept
{
    if (!apBase || apBase->IsTemporary())
        return false;

    const TESForm* pTemplate = apBase->actorData.baseTemplateForm;
    if (pTemplate && pTemplate->formType == FormType::LeveledCharacter)
        return true;

#if defined(TP_FALLOUT4)
    // Fallout 4 can also template each use flag from its own form.
    if (apBase->actorData.templateUseFlags && apBase->actorData.templateForms)
    {
        for (uint32_t i = 0; i < kTemplateSlotCount; ++i)
        {
            pTemplate = apBase->actorData.templateForms[i];
            if (pTemplate && pTemplate->formType == FormType::LeveledCharacter)
                return true;
        }
    }
#endif

    return false;
}

TESNPC* LeveledNpcSystem::GetOriginalBase(const Actor* apActor) noexcept
{
    if (!apActor)
        return nullptr;

#if defined(TP_FALLOUT4)
    const ExtraDataList* pExtraList = apActor->extraData;
    const auto* pExtra = pExtraList ? static_cast<ExtraLeveledCreature*>(pExtraList->GetByType(ExtraDataType::LeveledCreature)) : nullptr;
#else
    const auto* pExtra = static_cast<ExtraLeveledCreature*>(apActor->extraData.GetByType(ExtraDataType::LeveledCreature));
#endif
    if (pExtra && pExtra->originalBase)
        return Cast<TESNPC>(pExtra->originalBase);

    auto* pBase = Cast<TESNPC>(apActor->baseForm);
    return IsLeveledNpcBase(pBase) ? pBase : nullptr;
}

bool LeveledNpcSystem::ApplyPick(Actor* apActor, TESNPC* apPick) noexcept
{
    if (!apPick)
        return false;

#if defined(TP_FALLOUT4)
    auto* pOriginalBase = GetOriginalBase(apActor);
    if (!pOriginalBase)
        return false;

    // The engine keeps one template per use flag, and use flags templated from the same leveled list share a
    // pick. Only one pick is synced, so it replaces every slot that shares the local identity pick.
    TESActorBase* templates[kTemplateSlotCount]{};
    const ExtraDataList* pExtraList = apActor->extraData;
    const auto* pExtra = pExtraList ? static_cast<ExtraLeveledCreature*>(pExtraList->GetByType(ExtraDataType::LeveledCreature)) : nullptr;
    const TESNPC* pLocalPick = apActor->GetLeveledPick();
    for (uint32_t i = 0; i < kTemplateSlotCount; ++i)
    {
        if (pExtra && pLocalPick)
            templates[i] = pExtra->templateBases[i] == pLocalPick ? apPick : pExtra->templateBases[i];
        else if (pOriginalBase->actorData.templateUseFlags & (1 << i))
            templates[i] = apPick;
    }

    auto* pResolvedBase = TESActorBaseData::CreateTemplateActorBase(pOriginalBase, templates);
    if (!pResolvedBase)
        return false;

    auto* pOldBase = Cast<TESNPC>(apActor->baseForm);
    apActor->SetLeveledCreature(pOriginalBase, templates);
    apActor->SetObjectReference(pResolvedBase);

    if (pOldBase && pOldBase->IsTemporary() && pOldBase != pOriginalBase && pOldBase != apPick)
        DeferredDeleter::Add(pOldBase);

    spdlog::info("Applied leveled NPC pick for actor {:X}, original base: {:X}, base: {:X}, pick: {:X}",
        apActor->formID, pOriginalBase->formID, pResolvedBase->formID, apPick->formID);
    return true;
#else
    // Skyrim resolves a leveled NPC by copying the original base, then
    // applying the pick according to that base's template flags. Using
    // the pick itself discards data such as a hold guard's name/outfit.
    auto* pOriginalBase = GetOriginalBase(apActor);
    auto* pResolvedBase = pOriginalBase ? TESActorBaseData::CreateTemplateActorBase(pOriginalBase, apPick) : nullptr;
    if (!pResolvedBase)
        return false;

    auto* pOldBase = Cast<TESNPC>(apActor->baseForm);
    apActor->SetLeveledCreature(pOriginalBase, apPick);
    apActor->SetObjectReference(pResolvedBase);

    // Match RecalcLeveledActor's disposal policy, but never dispose of
    // a static pick left by the old reconciliation implementation.
    if (pOldBase && pOldBase->IsTemporary() && pOldBase != pOriginalBase && pOldBase != apPick)
        GarbageCollector::Get()->Add(pOldBase);

    spdlog::info("Applied leveled NPC pick for actor {:X}, original base: {:X}, base: {:X}, pick: {:X}",
        apActor->formID, pOriginalBase->formID, pResolvedBase->formID, apPick->formID);
    return true;
#endif
}
