#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>

struct TESObjectARMO;

struct BGSSkinForm : BaseFormComponent
{
    TESObjectARMO* armo;
};


#endif
