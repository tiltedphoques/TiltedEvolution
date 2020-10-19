#pragma once

#include <Forms/TESForm.h>
#include <Misc/BSFixedString.h>

struct BGSKeyword : TESForm
{
    virtual ~BGSKeyword();

    BSFixedString keyword;
};

