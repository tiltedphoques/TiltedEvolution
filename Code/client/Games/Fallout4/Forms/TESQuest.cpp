#include <TiltedOnlinePCH.h>

#include <Forms/TESQuest.h>

void TESQuest::SetActive(bool toggle)
{
    // bethesda implemented a new event dispatcher
    // whenever a quest is activated, that's why
    // we use the game function instead of reimplementing it.
    using TSetActive = void(TESQuest*, bool);
    POINTER_FALLOUT4(TSetActive, SetActive, 1567563);

    SetActive(this, toggle);
}

void TESQuest::SetStopped()
{
    flags &= 0xFFFE;
    MarkChanged(2);
}

bool TESQuest::UnkSetRunning(bool &success, bool force)
{
    using TSetRunning = bool(TESQuest*, bool*, bool);
    POINTER_FALLOUT4(TSetRunning, SetRunning, 966434);

    return SetRunning(this, &success, force);
}

bool TESQuest::SetStage(uint16_t stage)
{
    using TSetStage = bool(TESQuest*, uint16_t);
    POINTER_FALLOUT4(TSetStage, SetStage, 952800);

    return SetStage(this, stage);
}
