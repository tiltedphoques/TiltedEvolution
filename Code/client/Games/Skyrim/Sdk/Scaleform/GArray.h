#pragma once

#include "Sdk/Scaleform/GAllocator.h"

namespace Scaleform
{
struct ArrayDefaultPolicy
{
    size_t Capacity;
};

template <typename TSize, class TAllocator> struct ArrayDataBase
{
    TSize* Data;
    size_t Size;
    Scaleform::ArrayDefaultPolicy Policy;
};

template <typename TSize, class TAllocator, typename TPolicy> struct ArrayData : ArrayDataBase<TSize, TAllocator>
{
};

template <class TData> struct ArrayBase
{
    TData Data;
};

template <typename T, size_t TSize, class TPolicy = ArrayDefaultPolicy>
struct Array : Scaleform::ArrayBase<ArrayData<T, Scaleform::AllocatorGH<T, TSize>, TPolicy>>
{
};
} // namespace Scaleform
