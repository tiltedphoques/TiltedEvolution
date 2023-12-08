#include "Memory.h"

#pragma optimize("", off)

struct MemoryManager
{
    static MemoryManager* Get()
    {
        using TGetMemoryManager = MemoryManager*(__fastcall)();
        TGetMemoryManager* getMemoryManager = (TGetMemoryManager*)0x140546A94; // 33961
        return getMemoryManager();
    }
};

using TFormAllocate = void*(__fastcall)(MemoryManager*, size_t aSize, size_t aAlignment, bool aAligned);
using TFormFree = void(__fastcall)(MemoryManager*, void* apPtr, bool aAligned);

TFormAllocate* RealFormAllocate = nullptr;
TFormFree* RealFormFree = nullptr;

void* __fastcall HookFormAllocate(MemoryManager* apThis, size_t aSize, size_t aAlignment, bool aAligned)
{
    #if 0
    switch (aSize)
    {
    case sizeof(Actor): aSize = sizeof(ExActor); break;
    case sizeof(PlayerCharacter): aSize = sizeof(ExPlayerCharacter); break;
    default: break;
    }
    #endif

    //auto* pPointer = TiltedPhoques::ThisCall(RealFormAllocate, apThis, aSize, aAlignment, aAligned);
    auto* pPointer = RealFormAllocate(apThis, aSize, aAlignment, aAligned);

    #if 0
    if (!pPointer)
        return nullptr;

    ActorExtension* pExtension = nullptr;

    switch (aSize)
    {
    case sizeof(ExActor): pExtension = static_cast<ActorExtension*>(static_cast<ExActor*>(pPointer)); break;
    case sizeof(ExPlayerCharacter): pExtension = static_cast<ActorExtension*>(static_cast<ExPlayerCharacter*>(pPointer)); break;
    default: break;
    }

    if (pExtension)
    {
        new (pExtension) ActorExtension;
    }
    #endif

    return pPointer;
}

void* Memory::Allocate(const size_t aSize) noexcept
{
    // TODO: aligned? size?
    return HookFormAllocate(MemoryManager::Get(), aSize, 0, false);
}

void Memory::Free(void* apData) noexcept
{
    // TODO: aligned?
    RealFormFree(MemoryManager::Get(), apData, false);
}

#if 0
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

static TiltedPhoques::Initializer s_memoryHooks(
    []()
    {
        TFormAllocate* s_formAllocate = (TFormAllocate*)0x140546AB4; // 33962
        TFormFree* s_formFree = (TFormFree*)0x14055AF14; // 34032

        RealFormAllocate = s_formAllocate;
        RealFormFree = s_formFree;

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
#endif

#pragma optimize("", on)
