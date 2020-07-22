#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/Tints.h>

struct InventoryComponent
{
    String InventoryBuffer{};
    bool DirtyInventory{false};
};
