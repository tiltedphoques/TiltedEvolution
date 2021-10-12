#include <stdafx.h>


#include <World.h>
#include <Scripts/Party.h>
#include <Scripts/Player.h>
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

        auto& partyService = m_pWorld->ctx<PartyService>();

        auto* pParty = partyService.GetById(m_partyId);
        if (pParty)
        {
            //TODO: Fix script 
            /* for (auto entity : pParty->Members)
            {
                players.emplace_back(entity, *m_pWorld);
            }*/
        }
        
        return players;
    }
}
