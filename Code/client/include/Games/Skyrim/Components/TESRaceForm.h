#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESForm.h>

struct TESRace;

struct TESRaceForm : BaseFormComponent
{
    TESRace* race;
};

#endif
