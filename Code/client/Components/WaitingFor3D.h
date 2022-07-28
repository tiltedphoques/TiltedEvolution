#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Messages/CharacterSpawnRequest.h>

struct WaitingFor3D
{
    WaitingFor3D(const CharacterSpawnRequest& aSpawnRequest)
        : SpawnRequest(aSpawnRequest)
    {}

    CharacterSpawnRequest SpawnRequest{};
};
