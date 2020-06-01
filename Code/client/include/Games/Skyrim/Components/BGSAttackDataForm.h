#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>

struct BGSAttackDataMap : NiRefObject
{

};

struct BGSAttackDataForm : BaseFormComponent
{
    BGSAttackDataMap* attackDataMap;
};

#endif
