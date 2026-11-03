#pragma once

#include <ExtraData.h>

struct TESForm;

// Layout from CommonLibSSE-NG (ExtraOwnership): type 0x21, owner right after the BSExtraData header
struct ExtraOwnership : BSExtraData
{
    inline static constexpr auto eExtraData = ExtraDataType::Ownership;

    virtual ~ExtraOwnership();

    TESForm* owner;
};

static_assert(sizeof(ExtraOwnership) == 0x18);
static_assert(offsetof(ExtraOwnership, owner) == 0x10);
