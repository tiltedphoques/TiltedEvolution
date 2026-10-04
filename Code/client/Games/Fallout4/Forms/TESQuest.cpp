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

// Papyrus is not bridged on Fallout 4 yet, so this follows Quest.SetCurrentStageID
// natively: start the quest, then set the stage once it is running.
void TESQuest::ScriptSetStage(uint16_t aStageIndex)
{
    if (currentStage == aStageIndex || IsStageDone(aStageIndex))
        return;

    bool startDelayed = false;
    if (!EnsureQuestStarted(startDelayed, false))
        return;

    if (startDelayed)
    {
        spdlog::warn("Quest {:X} is starting asynchronously, stage {} not applied", formID, aStageIndex);
        return;
    }

    SetStage(aStageIndex);
}

void TESQuest::SetStopped()
{
    TP_THIS_FUNCTION(TSetEnabled, void, TESQuest, bool);
    static VersionDbPtr<TSetEnabled> setEnabled(2207727);
    TiltedPhoques::ThisCall(setEnabled, this, false);
}
