#pragma once

#include <Components/BaseFormComponent.h>
#include <Games/Skyrim/NetImmerse/NiPointer.h>

struct BGSAttackDataMap : NiRefObject
{
};

struct BGSAttackDataForm : BaseFormComponent
{
    NiPointer<BGSAttackDataMap> attackDataMap;
};
