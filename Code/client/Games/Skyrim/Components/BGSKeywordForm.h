#pragma once

#include <Components/BaseFormComponent.h>

struct BGSKeyword;

struct BGSKeywordForm : BaseFormComponent
{
    virtual bool Contains(BGSKeyword* apKeyword) const;
    virtual void sub_5();

    BGSKeyword** keywords;
    uint32_t count;
};
