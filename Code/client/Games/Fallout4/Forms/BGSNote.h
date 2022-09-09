#pragma once

#include "TESBoundObject.h"
#include "TESWeightForm.h"
#include "TESValueForm.h"
#include <Components/TESModel.h>

struct BGSNote : TESBoundObject, TESWeightForm, TESValueForm, TESModel, TESFullName
{

};
