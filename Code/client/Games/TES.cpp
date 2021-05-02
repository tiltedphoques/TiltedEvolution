#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

DataHandler* DataHandler::Get() noexcept
{
    // TODO: WRONG ADDRESS!
    POINTER_SKYRIMSE(DataHandler*, dataHandler, 0x141EC0A50 - 0x140000000);
    POINTER_FALLOUT4(DataHandler*, dataHandler, 0x1458CF080 - 0x140000000);

    return *dataHandler.Get();
}

TESObjectCELL* DataHandler::GetCellFromCoordinates(DataHandler* aDataHandler, int32_t aX, int32_t aY, TESWorldSpace* aWorldSpace, bool aSpawnCell) noexcept
{
    TP_THIS_FUNCTION(TDataHandler, TESObjectCELL*, DataHandler, int32_t, int32_t, TESWorldSpace*, bool);
    POINTER_SKYRIMSE(TDataHandler, getCell, 0x141EC0A50 - 0x140000000);
    POINTER_FALLOUT4(TDataHandler, getCell, 0x140113480 - 0x140000000);

    return ThisCall(getCell, aDataHandler, aX, aY, aWorldSpace, aSpawnCell);
}

TES* TES::Get() noexcept
{
    POINTER_SKYRIMSE(TES*, tes, 0x141EC0A50 - 0x140000000);
    POINTER_FALLOUT4(TES*, tes, 0x145AA4288 - 0x140000000);

    return *tes.Get();
}

ActorHolder* ActorHolder::Get() noexcept
{
    POINTER_SKYRIMSE(ActorHolder*, holder, 0x141EBEAD0 - 0x140000000);
    POINTER_FALLOUT4(ActorHolder*, holder, 0x1458CEE98 - 0x140000000);

    return *holder.Get();
}

