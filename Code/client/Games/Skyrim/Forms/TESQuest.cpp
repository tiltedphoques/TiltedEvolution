#include <TiltedOnlinePCH.h>

#include <Forms/TESQuest.h>

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
    POINTER_SKYRIMSE(TSetCompleted, SetCompleted, 0x140387960 - 0x140000000);

    SetCompleted(this, force);
}

void TESQuest::CompleteAllObjectives()
{
    using TCompleteAllObjectives = void(TESQuest*);
    POINTER_SKYRIMSE(TCompleteAllObjectives, CompleteAll, 0x140339BD0 - 0x140000000);

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
    POINTER_SKYRIMSE(TSetStopped, SetStopped, 0x1403876C0 - 0x140000000);

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
    POINTER_SKYRIMSE(TSetRunning, SetRunning, 0x140387FE0 - 0x140000000);

    return SetRunning(this, &success, force);
}

bool TESQuest::SetStage(uint16_t newStage)
{
    using TSetStage = bool(TESQuest*, uint16_t);
    POINTER_SKYRIMSE(TSetStage, SetStage, 0x1403880F0 - 0x140000000);

    return SetStage(this, newStage);
}

void TESQuest::SetStopped()
{
    flags &= 0xFFFE;
    MarkChanged(2);
}

static TiltedPhoques::Initializer s_questInitHooks([]() {
    // kill quest init in cold blood
    //TiltedPhoques::Write<uint8_t>(0x140387FE0 - 0x140000000, 0xC3);
});
