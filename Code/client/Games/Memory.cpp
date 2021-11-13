#include <TiltedOnlinePCH.h>

#include <Games/Memory.h>
#include <Games/References.h>

#include <TiltedCore/MimallocAllocator.hpp>
#include <mimalloc.h>

#pragma optimize( "", off )

struct GameHeap;

TP_THIS_FUNCTION(TFormAllocate, void*, GameHeap, size_t aSize, size_t aAlignment, bool aAligned);
TP_THIS_FUNCTION(TFormFree, void, GameHeap, void* apPtr, bool aAligned);

POINTER_SKYRIMSE(GameHeap, s_gameHeap, 0x141F57A80 - 0x140000000);
POINTER_FALLOUT4(GameHeap, s_gameHeap, 0x1438CC980 - 0x140000000);

POINTER_SKYRIMSE(TFormAllocate, s_formAllocate, 0x140C26F00 - 0x140000000);
POINTER_FALLOUT4(TFormAllocate, s_formAllocate, 0x141B0EFD0 - 0x140000000);

POINTER_SKYRIMSE(TFormFree, s_formFree, 0x140C27350 - 0x140000000);
POINTER_FALLOUT4(TFormFree, s_formFree, 0x141B0F2E0 - 0x140000000);

TFormAllocate* RealFormAllocate = nullptr;
TFormFree* RealFormFree = nullptr;

static TiltedPhoques::MimallocAllocator s_allocator;

void* TP_MAKE_THISCALL(HookFormAllocate, GameHeap, size_t aSize, size_t aAlignment, bool aAligned)
{
    switch (aSize)
    {
        case sizeof(Actor): aSize = sizeof(ExActor); break;
        case sizeof(PlayerCharacter) : aSize = sizeof(ExPlayerCharacter); break;
        default: break;
    }

    auto* pPointer = ThisCall(RealFormAllocate, apThis, aSize, aAlignment, aAligned);

    if (!pPointer)
        return nullptr;

    ActorExtension* pExtension = nullptr;

    switch (aSize)
    {
        case sizeof(ExActor) : pExtension = static_cast<ActorExtension*>(static_cast<ExActor*>(pPointer)); break;
        case sizeof(ExPlayerCharacter) : pExtension = static_cast<ActorExtension*>(static_cast<ExPlayerCharacter*>(pPointer));  break;
        default: break;
    }

    if(pExtension)
    {
        new (pExtension) ActorExtension;
    }

    return pPointer;
}

void* Memory::Allocate(const size_t aSize) noexcept
{
#if TP_FALLOUT4
    return ThisCall(HookFormAllocate, s_gameHeap, aSize, 0x10, true);
#else
    return ThisCall(HookFormAllocate, s_gameHeap, aSize, 0, false);
#endif
}

void Memory::Free(void* apData) noexcept
{
#if TP_FALLOUT4
    ThisCall(RealFormFree, s_gameHeap, apData, true);
#else
    ThisCall(RealFormFree, s_gameHeap, apData, false);
#endif
}

size_t Hook_msize(void* apData)
{
    return mi_malloc_size(apData);
}

void Hookfree(void* apData)
{
    mi_free(apData);
}

void* Hookcalloc(size_t aCount, size_t aSize)
{
    return mi_calloc(aCount, aSize);
}

void* Hookmalloc(size_t aSize)
{
    return mi_malloc(aSize);
}

void Hook_aligned_free(void* apData)
{
    mi_free(apData);
}

void* Hook_aligned_malloc(size_t aSize, size_t aAlignment)
{
    return mi_malloc_aligned(aSize, aAlignment);
}

static TiltedPhoques::Initializer s_memoryHooks([]()
    {
        RealFormAllocate = s_formAllocate.Get();
        RealFormFree = s_formFree.Get();

#if TP_PLATFORM_64
        using T_msize = decltype(&Hook_msize);
        using Tfree = decltype(&Hookfree);
        using Tcalloc = decltype(&Hookcalloc);
        using Tmalloc = decltype(&Hookmalloc);
        using T_aligned_malloc = decltype(&Hook_aligned_malloc);
        using T_aligned_free = decltype(&Hook_aligned_free);
        T_msize Real_msize = nullptr;
        Tfree Realfree = nullptr;
        Tcalloc Realcalloc = nullptr;
        Tmalloc Realmalloc = nullptr;
        T_aligned_malloc Real_aligned_malloc = nullptr;
        T_aligned_free Real_aligned_free = nullptr;

#if TP_FALLOUT4
        const char* cModuleName = "MSVCR110.dll";
#else
        const char* cModuleName = "api-ms-win-crt-heap-l1-1-0.dll";
#endif

        TP_HOOK_IAT(_msize, cModuleName);
        TP_HOOK_IAT(free, cModuleName);
        TP_HOOK_IAT(calloc, cModuleName);
        TP_HOOK_IAT(malloc, cModuleName);
        TP_HOOK_IAT(_aligned_malloc, cModuleName);
        TP_HOOK_IAT(_aligned_free, cModuleName);
#endif

        TP_HOOK(&RealFormAllocate, HookFormAllocate);
    });

#pragma optimize( "", on )
