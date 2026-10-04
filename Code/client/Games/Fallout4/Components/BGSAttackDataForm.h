#pragma once

#include <Components/BaseFormComponent.h>
#include <Games/Fallout4/NetImmerse/NiPointer.h>

struct BGSAttackDataMap : NiRefObject
{
};

struct BGSAttackDataForm : BaseFormComponent
{
    NiPointer<BGSAttackDataMap> attackDataMap;
};
