#pragma once

#include <Scripting/Player.h>
#include <Structs/GameId.h>

namespace Script
{
struct Quest
{
    explicit Quest(GameId aId, uint16_t aStage, World& aWorld)
        : m_id(aId)
        , m_stage(aStage)
        , m_pWorld(&aWorld)
    {
    }

    explicit Quest(GameId aId, uint16_t aStage, World* aWorld)
        : m_id(aId)
        , m_stage(aStage)
        , m_pWorld(aWorld)
    {
    }

    [[nodiscard]] GameId GetId() const { return m_id; }

    [[nodiscard]] uint16_t GetStage() const { return m_stage; }

    void SetStage(uint16_t aNewStage, std::vector<Player> aPlayers);

private:
    // in order to keep quests copyable we need
    // to store the world as a pointer and non reference
    World* m_pWorld;
    GameId m_id{};
    uint16_t m_stage = 0;
};
} // namespace Script
