#pragma once

struct World;

namespace Script
{
    struct Player;

    struct Party
    {
        explicit Party(uint32_t aId, World& aWorld) : 
            m_partyId(aId), m_pWorld(&aWorld)
        {}

        [[nodiscard]] uint32_t GetId() const noexcept;
        [[nodiscard]] Vector<Player> GetPlayers() const noexcept;

    private:
        
        World* m_pWorld;
        uint32_t m_partyId;
    };
}
