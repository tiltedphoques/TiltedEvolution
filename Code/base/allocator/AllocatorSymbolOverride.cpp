// Purposefully in no namespace.

#include <TiltedCore/Allocator.hpp>

void* TPAlloc(const size_t acSize)
{
    return TiltedPhoques::Allocator::Get()->Allocate(acSize);
}

void TPFree(void* apBlock)
{
    TiltedPhoques::Allocator::Get()->Free(apBlock);
}

void* operator new(size_t size)
{
    return TPAlloc(size);
}

void operator delete(void* p) noexcept
{
    TPFree(p);
}

void* operator new[](size_t size)
{
    return TPAlloc(size);
}

void operator delete[](void* p) noexcept
{
    TPFree(p);
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    return TPAlloc(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    return TPAlloc(size);
}

void operator delete(void* p, const std::nothrow_t&) noexcept
{
    TPFree(p);
}

void operator delete[](void* p, const std::nothrow_t&) noexcept
{
    TPFree(p);
}

void operator delete(void* p, size_t) noexcept
{
    TPFree(p);
}

void operator delete[](void* p, size_t) noexcept
{
    TPFree(p);
}