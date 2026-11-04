#include <Forms/TESQuest.h>

TESObjectREFR* TESQuest::GetAliasedRef(uint32_t aAliasID) noexcept
{
    TP_THIS_FUNCTION(TGetAliasedRef, BSPointerHandle<TESObjectREFR>*, TESQuest, BSPointerHandle<TESObjectREFR>*, uint32_t);
    static VersionDbPtr<TGetAliasedRef> getAliasedRef(2207810);

    BSPointerHandle<TESObjectREFR> result{};
    TiltedPhoques::ThisCall(getAliasedRef, this, &result, aAliasID);

    return TESObjectREFR::GetByHandle(result.handle.iBits);
}

bool TESQuest::IsStageDone(uint16_t aStageIndex)
{
    TP_THIS_FUNCTION(TIsStageDone, bool, TESQuest, uint16_t);
    static VersionDbPtr<TIsStageDone> isStageDone(2207744);
    return TiltedPhoques::ThisCall(isStageDone, this, aStageIndex);
}

void TESQuest::SetActive(bool aActive)
{
    TP_THIS_FUNCTION(TSetActive, void, TESQuest, bool);
    static VersionDbPtr<TSetActive> setActive(2207734);
    TiltedPhoques::ThisCall(setActive, this, aActive);
}

bool TESQuest::EnsureQuestStarted(bool& aStartDelayed, bool aImmediate)
{
    TP_THIS_FUNCTION(TEnsureQuestStarted, bool, TESQuest, bool*, bool);
    static VersionDbPtr<TEnsureQuestStarted> ensureQuestStarted(2207742);
    return TiltedPhoques::ThisCall(ensureQuestStarted, this, &aStartDelayed, aImmediate);
}

bool TESQuest::SetStage(uint16_t aStageIndex)
{
    TP_THIS_FUNCTION(TSetStage, bool, TESQuest, uint16_t);
    static VersionDbPtr<TSetStage> setStage(2207743);
    return TiltedPhoques::ThisCall(setStage, this, aStageIndex);
}

namespace
{
TiltedPhoques::Map<uint32_t, uint16_t> s_pendingStages;
}

// Papyrus is not bridged on Fallout 4 yet, so this follows Quest.SetCurrentStageID
// natively: start the quest, then set the stage. Starting is deferred to a task, so
// like Papyrus the stage waits for the quest's start event.
bool TESQuest::ScriptSetStage(uint16_t aStageIndex)
{
    if (IsStageReached(aStageIndex))
        return false;

    bool startDelayed = false;
    if (!EnsureQuestStarted(startDelayed, false))
        return false;

    if (startDelayed)
    {
        s_pendingStages[formID] = aStageIndex;
        return false;
    }

    SetStage(aStageIndex);
    return IsStageReached(aStageIndex);
}

bool TESQuest::IsStageReached(uint16_t aStage) noexcept
{
    return currentStage == aStage || IsStageDone(aStage);
}

uint32_t TESQuest::GetStageItemCount(uint16_t aStage) noexcept
{
    for (Stage* pStage : stages)
    {
        if (!pStage || pStage->stageIndex != aStage)
            continue;

        uint32_t count = 0;
        for (void* pItem : pStage->items)
        {
            if (pItem)
                ++count;
        }
        return count;
    }

    return 0;
}

void TESQuest::ApplyPendingStage(uint32_t aFormId) noexcept
{
    const auto it = s_pendingStages.find(aFormId);
    if (it == s_pendingStages.end())
        return;

    const uint16_t stage = it->second;
    s_pendingStages.erase(it);

    if (auto* pQuest = Cast<TESQuest>(TESForm::GetById(aFormId)))
        pQuest->SetStage(stage);
}

void TESQuest::SetStopped()
{
    TP_THIS_FUNCTION(TSetEnabled, void, TESQuest, bool);
    static VersionDbPtr<TSetEnabled> setEnabled(2207727);
    TiltedPhoques::ThisCall(setEnabled, this, false);
}
