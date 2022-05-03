#include <Forms/TESQuest.h>

#include <Services/PapyrusService.h>

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
    using TSetCompleted = void(TESQuest*, bool);
    POINTER_SKYRIMSE(TSetCompleted, SetCompleted, 24991);

    SetCompleted(this, force);
}

void TESQuest::CompleteAllObjectives()
{
    using TCompleteAllObjectives = void(TESQuest*);
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
    for (auto* it : stages)
    {
        if (it->stageIndex == stageIndex)
            return it->IsDone();
    }

    return false;
}

bool TESQuest::Kill()
{
    using TSetStopped = void(TESQuest*, bool);
    POINTER_SKYRIMSE(TSetStopped, SetStopped, 24987);

    if (flags & Flags::Enabled)
    {
        unkFlags = 0;
        flags = Flags::Disabled;
        MarkChanged(2);

        //SetStopped(this, false);
        return true;
    }

    return false;
}

bool TESQuest::UnkSetRunning(bool &success, bool force)
{
    using TSetRunning = bool(TESQuest*, bool*, bool);
    POINTER_SKYRIMSE(TSetRunning, SetRunning, 25003);

    return SetRunning(this, &success, force);
}

bool TESQuest::SetStage(uint16_t newStage)
{
    using TSetStage = bool(TESQuest*, uint16_t);
    POINTER_SKYRIMSE(TSetStage, SetStage, 25004);

    return SetStage(this, newStage);
}

void TESQuest::ScriptSetStage(uint16_t stage)
{
    using Quest = TESQuest;
    PAPYRUS_FUNCTION(void, Quest, SetCurrentStageID, int);
    s_pSetCurrentStageID(this, stage);
}

void TESQuest::SetStopped()
{
    flags &= 0xFFFE;
    MarkChanged(2);
}

static TiltedPhoques::Initializer s_questInitHooks([]() {
    // kill quest init in cold blood
    //TiltedPhoques::Write<uint8_t>(25003, 0xC3);
});
