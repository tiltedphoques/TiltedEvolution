#pragma once

#include <Components/BaseFormComponent.h>

struct LoadedCellData
{
    uint8_t pad160[0x160]; // Offset
    BGSEncounterZone* encounterZone;
};

static_assert(offsetof(LoadedCellData, encounterZone) == 0x160);
