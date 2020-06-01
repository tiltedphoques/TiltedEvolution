#pragma once

struct CharacterSpawnedEvent
{
    CharacterSpawnedEvent(const entt::entity aEntity)
        : Entity{aEntity}
    {}

    entt::entity Entity;
};
