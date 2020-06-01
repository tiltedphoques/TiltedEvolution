#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESBoundAnimObject.h>

#include <Games/Skyrim/Components/TESContainer.h>
#include <Games/Skyrim/Components/TESFullName.h>

struct TESObjectCONT : TESBoundAnimObject
{
    TESContainer container;
    TESFullName    fullName;
    // more stuff
};

#endif
