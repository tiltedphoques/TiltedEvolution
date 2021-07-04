#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct Player;
struct OwnerComponent
{
    OwnerComponent(Player* apPlayer) : pOwner(apPlayer)
    {}


    Player* GetOwner() const
    {
        return reinterpret_cast<Player*>(pOwner);    
    }

    void SetOwner(Player* apPlayer)
    {
        pOwner = apPlayer;
    }

    Player* pOwner;
    Vector<const Player*> InvalidOwners{};
    
};
