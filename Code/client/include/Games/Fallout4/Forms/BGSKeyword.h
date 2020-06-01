#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Forms/TESForm.h>
#include <Games/Fallout4/Misc/BSFixedString.h>

struct BGSKeyword : TESForm
{
    virtual ~BGSKeyword();

    BSFixedString keyword;
};

#endif
