#pragma once

#include "TESForm.h"

struct TESHandleForm : TESForm
{
};

static_assert(sizeof(TESHandleForm) == 0x38);
