#include <Forms/TESQuest.h>

void TESQuest::SetActive(bool toggle)
{
    // bethesda implemented a new event dispatcher
    // whenever a quest is activated, that's why
    // we use the game function instead of reimplementing it.
    using TSetActive = void(TESQuest*, bool);
    POINTER_FALLOUT4(TSetActive, SetActive, 0x1405D6AC0 - 0x140000000);

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
    POINTER_FALLOUT4(TSetRunning, SetRunning, 0x1405D70C0 - 0x140000000);

    return SetRunning(this, &success, force);
}

bool TESQuest::SetStage(uint16_t stage)
{
    using TSetStage = bool(TESQuest*, uint16_t);
    POINTER_FALLOUT4(TSetStage, SetStage, 0x1405D71E0 - 0x140000000);

    return SetStage(this, stage);
}
