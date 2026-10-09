#pragma once

#include <Components/BaseFormComponent.h>

struct BGSKeyword;
struct TBO_InstanceData;

struct IKeywordFormBase
{
    virtual ~IKeywordFormBase() = default;
    virtual bool HasKeyword(const BGSKeyword* apKeyword, const TBO_InstanceData* apData = nullptr) const;
    virtual void CollectAllKeywords(void* apArray, const TBO_InstanceData* apData) const;
};

struct BGSKeywordForm : BaseFormComponent, IKeywordFormBase
{
    virtual BGSKeyword* GetDefaultKeyword() const;

    bool Contains(BGSKeyword* apKeyword) const { return HasKeyword(apKeyword); }

    BGSKeyword** keywords;
    uint32_t count;
};

static_assert(offsetof(BGSKeywordForm, keywords) == 0x10);
static_assert(sizeof(BGSKeywordForm) == 0x20);
