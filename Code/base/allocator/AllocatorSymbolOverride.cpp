// Purposefully in no namespace.

#include <TiltedCore/Allocator.hpp>

// not private cause thse can be referenced as external symbols
void* TiltedAlloc(const size_t acSize)
{
    return TiltedPhoques::Allocator::GetDefault()->Allocate(acSize);
}

void TiltedFree(void* apBlock)
{
    TiltedPhoques::Allocator::GetDefault()->Free(apBlock);
}

void* operator new(size_t size)
{
    return TiltedAlloc(size);
}

void operator delete(void* p) noexcept
{
    TiltedFree(p);
}

void* operator new[](size_t size)
{
    return TiltedAlloc(size);
}

void operator delete[](void* p) noexcept
{
    TiltedFree(p);
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    return TiltedAlloc(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    return TiltedAlloc(size);
}

void operator delete(void* p, const std::nothrow_t&) noexcept
{
    TiltedFree(p);
}

void operator delete[](void* p, const std::nothrow_t&) noexcept
{
    TiltedFree(p);
}

void operator delete(void* p, size_t) noexcept
{
    TiltedFree(p);
}

void operator delete[](void* p, size_t) noexcept
{
    TiltedFree(p);
}
