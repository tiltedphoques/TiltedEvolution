#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>

struct BGSKeyword;

struct BGSKeywordForm : BaseFormComponent
{
    virtual bool Contains(BGSKeyword* apKeyword);
    virtual void sub_5();

    BGSKeyword** keywords;
    uint32_t count;
};

#endif
