#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Messages/CharacterSpawnRequest.h>
#include <Messages/NotifyEquipmentChanges.h>

struct WaitingFor3D
{
    WaitingFor3D(const CharacterSpawnRequest& aSpawnRequest)
        : SpawnRequest(aSpawnRequest)
    {
    }

    CharacterSpawnRequest SpawnRequest{};
    // Equipment changes relayed before the 3D loaded, replayed after the spawn request's inventory is applied
    Vector<NotifyEquipmentChanges> PendingEquipmentChanges{};
};
