
#include <atomic>
#include <cstdint>
#include <limits>
#include <mutex>

#include "utils/Error.h"

extern uint8_t highrip[0x100000];

static_assert(sizeof(highrip) == 0x100000, "Source file RipAllocator.cpp wasn't updated");
static_assert(sizeof(highrip) <= std::numeric_limits<uint32_t>::max(), "Highrip must fit within 4gib space");

namespace
{
uintptr_t g_bufferPointer{};
uintptr_t g_ListPointer{};
std::mutex g_lock;

struct FreeListHeader
{
    size_t m_magic{0xF03CEBAD};
    size_t m_count{0};
};

struct FreeListEntry
{
    enum
    {
        kAvailable = 1 << 0,
        kInUse = 1 << 1,
    };

    uint16_t m_size;
    uint16_t m_flags{kAvailable};
    uint32_t m_disp; // from start address
};

FreeListEntry* RipMemBlock(uint8_t* apBlock)
{
    uint8_t* pAddress = &highrip[sizeof(highrip) - sizeof(FreeListHeader)];

    FreeListHeader* pHeader = reinterpret_cast<FreeListHeader*>(pAddress);
    pAddress -= sizeof(FreeListEntry);

    FreeListEntry* pEntry = reinterpret_cast<FreeListEntry*>(pAddress);
    for (size_t i = 0; i < pHeader->m_count; i++)
    {
        uint8_t* fullAddress = &highrip[pEntry->m_disp];
        if (apBlock == fullAddress && pEntry->m_flags & FreeListEntry::kInUse)
            return pEntry;
        pEntry--;
    }
    return nullptr;
}

FreeListEntry* FindFreeEntry(const size_t aSize, size_t& used_size)
{
    uint8_t* pAddress = &highrip[sizeof(highrip) - sizeof(FreeListHeader)];
    FreeListHeader* pHeader = reinterpret_cast<FreeListHeader*>(pAddress);
    pAddress -= sizeof(FreeListEntry);

    FreeListEntry* pFoundEntry = nullptr;
    FreeListEntry* pEntry = reinterpret_cast<FreeListEntry*>(pAddress);
    for (size_t i = 0; i < pHeader->m_count; i++)
    {
        uint8_t* fullAddress = &highrip[pEntry->m_disp];
        if (!pFoundEntry && pEntry->m_size <= aSize && pEntry->m_flags & FreeListEntry::kAvailable)
        {
            pFoundEntry = pEntry;
        }

        used_size += pEntry->m_size;
        pEntry--;
    }
    return nullptr;
}
} // namespace

void RipAllocatorInit()
{
    g_bufferPointer = reinterpret_cast<uintptr_t>(&highrip[0]);

    uint8_t* pAddress = &highrip[sizeof(highrip) - sizeof(FreeListHeader)];
    // placement new
    new (pAddress) FreeListHeader();

    g_ListPointer = reinterpret_cast<uintptr_t>(pAddress);
}

void* RipAllocateN(size_t blockLength)
{
    if (blockLength > std::numeric_limits<uint16_t>::max())
    {
        Die(L"No more ripspace available :(", true);
        return nullptr;
    }

    std::scoped_lock _(g_lock);
    (void)_;

    size_t usedSize = 0;
    if (FreeListEntry* pEntry = FindFreeEntry(blockLength, usedSize))
    {
        // no need to update the disp
        pEntry->m_size = static_cast<uint32_t>(blockLength);

        // clear entry
        pEntry->m_flags &= ~FreeListEntry::kAvailable;

        return reinterpret_cast<void*>(&highrip[pEntry->m_disp]);
    }

    auto* pHeader = reinterpret_cast<FreeListHeader*>(&highrip[sizeof(highrip) - sizeof(FreeListHeader)]);

    size_t availableSize = sizeof(highrip);
    availableSize -= pHeader->m_count * sizeof(FreeListEntry);
    availableSize -= usedSize;
    if (blockLength > availableSize)
    {
        Die(L"No more available block size :(", true);
        return nullptr;
    }

    void* p = reinterpret_cast<void*>(g_bufferPointer);
    g_bufferPointer += blockLength;
    g_ListPointer -= sizeof(FreeListEntry);

    uint8_t* pListEntry = reinterpret_cast<uint8_t*>(g_ListPointer);

    uint32_t disp = static_cast<uint32_t>((uint8_t*)p - &highrip[0]);
    auto* pEntry = new (pListEntry) FreeListEntry{static_cast<uint16_t>(blockLength), FreeListEntry::kInUse, disp};
    pHeader->m_count++;

    return p;
}

void RipFree(uint8_t* apBlock)
{
    std::scoped_lock _(g_lock);
    (void)_;

    if (FreeListEntry* pEntry = RipMemBlock(apBlock))
    {
        pEntry->m_flags |= FreeListEntry::kAvailable;
    }
}
