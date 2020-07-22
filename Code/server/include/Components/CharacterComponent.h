#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/Tints.h>

struct CharacterComponent
{
    uint32_t ChangeFlags{ 0 };
    String SaveBuffer{};
    FormIdComponent BaseId{};
    String InventoryBuffer{};
    bool DirtyInventory{false};
    Tints FaceTints{};
};
