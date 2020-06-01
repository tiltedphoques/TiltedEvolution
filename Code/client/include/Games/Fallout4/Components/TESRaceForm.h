#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct TESRace;

struct TESRaceForm : BaseFormComponent
{
    virtual ~TESRaceForm();

    TESRace* race;
};

#endif
