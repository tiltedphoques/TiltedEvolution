#pragma once

#include "TESForm.h"

struct BGSStoryManagerTreeForm : TESForm
{
    virtual void sub_62();
    virtual void sub_63();
    virtual void sub_64();
    virtual void sub_65();

    uint8_t unkBGSStoryManagerTreeForm[0x40 - sizeof(TESForm)];
};

static_assert(sizeof(BGSStoryManagerTreeForm) == 0x40);