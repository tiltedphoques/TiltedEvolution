#include "TaskQueueInterface.h"

TaskQueueInterface* TaskQueueInterface::Get() noexcept
{
    POINTER_SKYRIMSE(TaskQueueInterface*, taskQueueInterface, 0x142FD3650 - 0x140000000);

    return *taskQueueInterface.Get();
}

void TaskQueueInterface::QueueApplyPerk(Actor* apActor, BGSPerk* apPerk, int8_t aOldRank, int8_t aNewRank) noexcept
{
    TP_THIS_FUNCTION(TQueueApplyPerk, void, TaskQueueInterface, Actor*, BGSPerk*, int8_t, int8_t);
    POINTER_SKYRIMSE(TQueueApplyPerk, queueApplyPerk, 0x1405EA750 - 0x140000000);
    ThisCall(queueApplyPerk, this, apActor, apPerk, aOldRank, aNewRank);
}
