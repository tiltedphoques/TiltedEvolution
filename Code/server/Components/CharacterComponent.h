#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/Tints.h>
#include <Structs/Factions.h>

struct CharacterComponent
{
    uint32_t ChangeFlags{ 0 };
    String SaveBuffer{};
    FormIdComponent BaseId{};
    Tints FaceTints{};
    Factions FactionsContent{};
    bool DirtyFactions{ false };
    bool IsDead{};
};
