#pragma once

#include <Components/BaseFormComponent.h>

struct BGSAttackDataMap : NiRefObject
{

};

struct BGSAttackDataForm : BaseFormComponent
{
    BGSAttackDataMap* attackDataMap;
};
