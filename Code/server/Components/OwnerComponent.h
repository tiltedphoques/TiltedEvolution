#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct Player;
struct OwnerComponent
{
    OwnerComponent(const Player* apPlayer) : pOwner(apPlayer)
    {}

    const Player* pOwner;
    Vector<const Player*> InvalidOwners{};
};
