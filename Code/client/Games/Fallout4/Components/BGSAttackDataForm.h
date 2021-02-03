#pragma once

#include <Components/BaseFormComponent.h>

struct BGSAttackDataMap : NiRefObject
{
    virtual ~BGSAttackDataMap();
};

struct BGSAttackDataForm : BaseFormComponent
{
    virtual ~BGSAttackDataForm();

    BGSAttackDataMap* attackDataMap;
};
