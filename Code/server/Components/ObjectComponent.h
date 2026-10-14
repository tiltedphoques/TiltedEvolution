#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/LockData.h>
#include <Game/Player.h>

struct ObjectComponent
{
    LockData CurrentLockData{};
    // Containers outlive the cell being empty, so whatever players stored in them is still there
    // when someone else (e.g. the host) enters the cell later on.
    bool IsContainer{};
    // Players whose local copy of a storage container has already been merged into the server state.
    TiltedPhoques::Set<String> MergedPlayers{};
};
