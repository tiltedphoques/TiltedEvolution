#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/LockData.h>
#include <Game/Player.h>
#include <optional>

struct ObjectComponent
{
    ObjectComponent(Player* apLastSender)
        : pLastSender(apLastSender)
    {
    }

    Player* pLastSender;
    std::optional<LockData> CurrentLockData{std::nullopt};
};
