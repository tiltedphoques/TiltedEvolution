#pragma once

#include <Components/BaseFormComponent.h>

struct BGSKeyword;

struct BGSKeywordForm : BaseFormComponent
{
    virtual ~BGSKeywordForm();

    void* vtbl2; // 8
    BGSKeyword** keywords; // 10
    uint32_t count; // 18
    uint32_t pad;
};
