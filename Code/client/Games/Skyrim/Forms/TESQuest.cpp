#include <Forms/TESQuest.h>

#include <Services/PapyrusService.h>

#include <Games/Overrides.h>

TESObjectREFR* TESQuest::GetAliasedRef(uint32_t aAliasID) noexcept
{
    TP_THIS_FUNCTION(TGetAliasedRef, BSPointerHandle<TESObjectREFR>*, TESQuest, BSPointerHandle<TESObjectREFR>*, uint32_t);
    POINTER_SKYRIMSE(TGetAliasedRef, getAliasedRef, 25066);

    BSPointerHandle<TESObjectREFR> result{};
    TiltedPhoques::ThisCall(getAliasedRef, this, &result, aAliasID);

    return TESObjectREFR::GetByHandle(result.handle.iBits);
}

TESQuest::State TESQuest::getState()
{
    if (flags >= 0)
    {
        if (unkFlags)
            return State::WaitingPromotion;
        else if (flags & 1)
            return State::Running;
        else
            return State::Stopped;
    }

    return State::WaitingForStage;
}

void TESQuest::SetCompleted(bool force)
{
    TP_THIS_FUNCTION(TSetCompleted, void, TESQuest, bool);
    POINTER_SKYRIMSE(TSetCompleted, SetCompleted, 24991);
    SetCompleted(this, force);
}

void TESQuest::CompleteAllObjectives()
{
    TP_THIS_FUNCTION(TCompleteAllObjectives, void, TESQuest);
    POINTER_SKYRIMSE(TCompleteAllObjectives, CompleteAll, 23231);
    CompleteAll(this);
}

void TESQuest::SetActive(bool toggle)
{
    if (toggle)
        flags |= 0x800;
    else
        flags &= 0xF7FF;
}

bool TESQuest::IsStageDone(uint16_t stageIndex)
{
    TP_THIS_FUNCTION(TIsStageDone, bool, TESQuest, uint16_t);
    POINTER_SKYRIMSE(TIsStageDone, IsStageDone, 25011);
    return IsStageDone(this, stageIndex);
}

bool TESQuest::Kill()
{
    using TSetStopped = void(TESQuest*, bool);
    POINTER_SKYRIMSE(TSetStopped, SetStopped, 24987);

    if (flags & Flags::Enabled)
    {
        unkFlags = 0;
        flags = Flags::Completed;
        MarkChanged(2);

        // SetStopped(this, false);
        return true;
    }

    return false;
}

bool TESQuest::EnsureQuestStarted(bool& success, bool force)
{
    TP_THIS_FUNCTION(TSetRunning, bool, TESQuest, bool*, bool);
    POINTER_SKYRIMSE(TSetRunning, SetRunning, 25003);
    return SetRunning(this, &success, force);
}

bool TESQuest::SetStage(uint16_t stageIndex)
{
    // According to wiki, calling newStage == currentStage does nothing.
    // Calling with newStage < currentStage, will rerun the stage actions
    // IFF the target newStage is not marked IsCompleted(). Regardless,
    // will not reduce currentStage (it stays the same).
    // Actually reducing currentStage requires reset() to be called first.
    TP_THIS_FUNCTION(TSetStage, bool, TESQuest, uint16_t);
    POINTER_SKYRIMSE(TSetStage, SetStage, 25004);
    bool bSuccess = SetStage(this, stageIndex);
    if (!bSuccess)
    {
        spdlog::warn(__FUNCTION__ ": returned false quest formId {:X}, currentStage {}, newStage {}, name {}", 
                     formID, currentStage, stageIndex, fullName.value.AsAscii());
    }
    return bSuccess;
}

bool TESQuest::ScriptSetStage(uint16_t stageIndex, bool bForce)
{
    // Since SetStage() rules are not well-known and hooks may be confused, filter rewind
    // according to TESQuest::SetStage rules.
    bool bSuccess = stageIndex > currentStage || stageIndex != currentStage && !IsStageDone(stageIndex) || bForce;

    if (bSuccess)
    {
        using Quest = TESQuest;
        PAPYRUS_FUNCTION(bool, Quest, SetCurrentStageID, int);
        bSuccess = s_pSetCurrentStageID(this, stageIndex);
    }

    if (!bSuccess)
    {
        spdlog::warn(__FUNCTION__ ": returned false quest formId {:X}, currentStage {}, newStage {}, name {}", formID, currentStage, stageIndex, fullName.value.AsAscii());
    }

    return bSuccess;
}

void TESQuest::ScriptReset()
{
    using Quest = TESQuest;
    PAPYRUS_FUNCTION(void, Quest, Reset);
    s_pReset(this);
}

void TESQuest::ScriptResetAndUpdate()
{
    ScriptReset();

    if (!IsEnabled())
    {
        if (!IsStopped())
            SetStopped();
    }

    else
    {
        bool isStarted;
        if (flags & StartsEnabled && !EnsureQuestStarted(isStarted, false))
            spdlog::warn(__FUNCTION__ ": EnsureQuestStarted failed questId {:X}, is Started {}", formID, isStarted);
    }
}


void TESQuest::SetStopped()
{
    flags &= 0xFFFE;
    MarkChanged(2);
}

bool TESQuest::IsAnyCutscenePlaying()
{
    for (const auto& scene : scenes)
    {
        if (scene->isPlaying)
            return true;
    }
    return false;
}

void BGSScene::ScriptForceStart()
{
    spdlog::info(__FUNCTION__ ": force starting scene questId {:X}, sceneId: {:X}, isPlaying? {}", owningQuest->formID, formID, isPlaying);
    using Scene = BGSScene;
    PAPYRUS_FUNCTION(void, Scene, ForceStart);
    s_pForceStart(this);
}

void BGSScene::ScriptStop()
{
    spdlog::info(__FUNCTION__ ": stopping scene questId {:X}, sceneId: {:X}, isPlaying? {}", owningQuest->formID, formID, isPlaying);
    using Scene = BGSScene;
    PAPYRUS_FUNCTION(void, Scene, Stop);
    s_pStop(this);
}

static TiltedPhoques::Initializer s_questInitHooks(
    []() 
    {
    // kill quest init in cold blood
    // TiltedPhoques::Write<uint8_t>(25003, 0xC3);
    });

