#pragma once

#include <Components/BaseFormComponent.h>

struct TESRace;

struct TESRaceForm : BaseFormComponent
{
    virtual ~TESRaceForm();

    TESRace* race;
};

