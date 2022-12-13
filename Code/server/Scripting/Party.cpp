#include <stdafx.h>


#include <World.h>
#include <Scripting/Party.h>
#include <Scripting/Player.h>
#include <Components.h>

#include <Services/PartyService.h>

namespace Script
{
    uint32_t Party::GetId() const noexcept
    {
        return m_partyId;
    }

    Vector<Player> Party::GetPlayers() const noexcept
    {
        Vector<Player> players;
        
        return players;
    }
}
