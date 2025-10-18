#include <TiltedOnlinePCH.h>

#include <Games/Memory.h>
#include <Games/References.h>

#include <TiltedCore/MimallocAllocator.hpp>
#include <mimalloc.h>

#pragma optimize("", off)

struct GameHeap
{
    static GameHeap* Get()
    {
        POINTER_SKYRIMSE(GameHeap, s_gameHeap, 400188);

        return s_gameHeap.Get();
    }
};

TP_THIS_FUNCTION(TFormAllocate, void*, GameHeap, size_t aSize, size_t aAlignment, bool aAligned);
TP_THIS_FUNCTION(TFormFree, void, GameHeap, void* apPtr, bool aAligned);

TFormAllocate* RealFormAllocate = nullptr;
TFormFree* RealFormFree = nullptr;

static TiltedPhoques::MimallocAllocator s_allocator;

void* TP_MAKE_THISCALL(HookFormAllocate, GameHeap, size_t aSize, size_t aAlignment, bool aAligned)
{
    switch (aSize)
    {
    case sizeof(Actor): aSize = sizeof(ExActor); break;
    case sizeof(PlayerCharacter): aSize = sizeof(ExPlayerCharacter); break;
    default: break;
    }

    auto* pPointer = TiltedPhoques::ThisCall(RealFormAllocate, apThis, aSize, aAlignment, aAligned);

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

    return pPointer;
}

void* Memory::Allocate(const size_t aSize) noexcept
{
    return TiltedPhoques::ThisCall(HookFormAllocate, GameHeap::Get(), aSize, 0, false);
}

void Memory::Free(void* apData) noexcept
{
    TiltedPhoques::ThisCall(RealFormFree, GameHeap::Get(), apData, false);
}

bool Memory::IsFormAllocateReplacedByEF() noexcept
{
    POINTER_SKYRIMSE(TFormAllocate, s_formAllocate, 68115);
    TFormAllocate* pFormAllocate = s_formAllocate.Get();

    // 6-byte sequence of 'FF 25 00 00 00 00' comes before the virtual address we're after; so, +6
    auto possibleEfAllocAddr = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8_t*>(*pFormAllocate) + 6);

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery((void*)possibleEfAllocAddr, &mbi, sizeof(mbi)) != 0) 
    {
        return mbi.AllocationBase == GetModuleHandleW(L"EngineFixes.dll");
    }
    return false;
}

bool Memory::RehookMemoryFunctions() noexcept
{
    POINTER_SKYRIMSE(TFormAllocate, s_formAllocate, 68115);
    TFormAllocate* pFormAllocate = s_formAllocate.Get();

    uintptr_t efAllocAddr = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8_t*>(*pFormAllocate) + 6);
    auto pEngineFixesAllocate = reinterpret_cast<decltype(&HookFormAllocate)>(efAllocAddr);

    RealFormAllocate = pEngineFixesAllocate;
    TP_HOOK_IMMEDIATE(&RealFormAllocate, HookFormAllocate);
    return true; // TODO when to return false?
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

static TiltedPhoques::Initializer s_memoryHooks(
    []()
    {
        POINTER_SKYRIMSE(TFormAllocate, s_formAllocate, 68115);

        POINTER_SKYRIMSE(TFormFree, s_formFree, 68117);

        RealFormAllocate = s_formAllocate.Get();
        RealFormFree = s_formFree.Get();

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

        const char* cModuleName = "api-ms-win-crt-heap-l1-1-0.dll";

        TP_HOOK_IAT(_msize, cModuleName);
        TP_HOOK_IAT(free, cModuleName);
        TP_HOOK_IAT(calloc, cModuleName);
        TP_HOOK_IAT(malloc, cModuleName);
        TP_HOOK_IAT(_aligned_malloc, cModuleName);
        TP_HOOK_IAT(_aligned_free, cModuleName);

        TP_HOOK(&RealFormAllocate, HookFormAllocate);
    });

using T_initterm_e = decltype(&_initterm_e);
T_initterm_e Real_initterm_e = nullptr;

// If EngineFixes loaded, and it changed our FormAllocate hook, 
// reset it. Our hook works just fine chaining to theirs.
int __cdecl Hook_initterm_e(_PIFV* apFirst, _PIFV* apLast)
{
    // We want to run last, so pre-chain.
    auto retval = Real_initterm_e(apFirst, apLast); 
    
    // Check if anyone messed with STR's moddified allocator hook,
    // which changes the size of some allocations.
    if (GetModuleHandleW(L"EngineFixes.dll") && Memory::IsFormAllocateReplacedByEF())
        Memory::RehookMemoryFunctions();

    return retval;
}

void HookFormAllocateSentinelInit()
{
    TP_HOOK_IAT(_initterm_e, "api-ms-win-crt-runtime-l1-1-0.dll");
}
#pragma optimize("", on)
