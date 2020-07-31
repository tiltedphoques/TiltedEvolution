#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/Components/TESFullName.h>

struct TESFaction : TESForm
{
    TESFullName fullname;
};

#endif
