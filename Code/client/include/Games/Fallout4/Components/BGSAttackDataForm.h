#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct BGSAttackDataMap : NiRefObject
{
    virtual ~BGSAttackDataMap();
};

struct BGSAttackDataForm : BaseFormComponent
{
    virtual ~BGSAttackDataForm();

    BGSAttackDataMap* attackDataMap;
};

#endif
