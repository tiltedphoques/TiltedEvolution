#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/Tints.h>

struct CharacterComponent
{
    uint32_t ChangeFlags;
    String SaveBuffer;
    FormIdComponent BaseId;
    String InventoryBuffer;
    Tints FaceTints;
};
