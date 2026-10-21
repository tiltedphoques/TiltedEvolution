#include "TESWorldSpace.h"

TESObjectCELL* TESWorldSpace::LoadCell(int32_t aXCoordinate, int32_t aYCoordinate) noexcept
{
    struct BGSSaveLoadGame
    {
        enum Flags : uint32_t
        {
            kLoading = 1 << 1,
            kInitingForms = 1 << 3,
        };

        uint8_t pad0[0x340];
        uint32_t flags;
    };

    TP_THIS_FUNCTION(TLoadCell, TESObjectCELL*, TESWorldSpace, int32_t aXCoordinate, int32_t aYCoordinate);
    TP_THIS_FUNCTION(TInitDeferredForms, void, BGSSaveLoadGame, bool aUnk);
    using TPromoteTasks = void();

    POINTER_SKYRIMSE(TLoadCell, s_loadCell, 20460);
    POINTER_SKYRIMSE(TPromoteTasks, s_pausePromoteTasks, 13333);
    POINTER_SKYRIMSE(TPromoteTasks, s_resumePromoteTasks, 13334);
    POINTER_SKYRIMSE(TInitDeferredForms, s_initDeferredForms, 35567);
    POINTER_SKYRIMSE(BGSSaveLoadGame*, s_saveLoadGame, 403330);

    // Loading a cell queues its references' saved changes on BGSSaveLoadGame without a lock, and the
    // QueuedPromoteReferencesTasks drain that queue on worker threads. Like the game's own cell loads, pause
    // those tasks and apply the queued changes before resuming, so no freed reference is left in the queue.
    s_pausePromoteTasks.Get()();

    TESObjectCELL* pCell = TiltedPhoques::ThisCall(s_loadCell, this, aXCoordinate, aYCoordinate);

    BGSSaveLoadGame* pSaveLoadGame = *s_saveLoadGame.Get();
    if (pSaveLoadGame && (pSaveLoadGame->flags & (BGSSaveLoadGame::kLoading | BGSSaveLoadGame::kInitingForms)) == 0)
        TiltedPhoques::ThisCall(s_initDeferredForms, pSaveLoadGame, false);

    s_resumePromoteTasks.Get()();

    return pCell;
}
