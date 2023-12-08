#pragma once

#include "BaseFormComponent.h"

struct TESFormRefCount : BaseFormComponent
{
    uint64_t IncRefCount() noexcept;
    uint64_t DecRefCount() noexcept;

	uint64_t refcount;
};

static_assert(sizeof(TESFormRefCount) == 0x10);
