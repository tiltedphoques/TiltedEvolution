// NOTE(V): Simple impl with negligleable overhead.

#include "RipAllocator.h"

#if USE_ALLOCATOR_V1

#include <atomic>
#include <cstdint>
#include <limits>

#include "utils/Error.h"

extern uint8_t highrip[0x100000];

static_assert(sizeof(highrip) == 0x100000, "Source file RipAllocator.cpp wasn't updated");
static_assert(sizeof(highrip) <= std::numeric_limits<uint32_t>::max(), "Highrip must fit within 4gib space");

static std::atomic<uintptr_t> g_bufferPointer;

void RipAllocatorInit()
{
    g_bufferPointer = reinterpret_cast<uintptr_t>(&highrip[0]);
}

void* RipAllocateN(size_t blockLength)
{
    if (blockLength > std::numeric_limits<uint16_t>::max())
    {
        Die(L"No more ripspace available :(", true);
        return nullptr;
    }

    // we might actually have to lock here :(
    uint8_t* pBuffer = reinterpret_cast<uint8_t*>(g_bufferPointer.load());

    // code should still be aligned at an 8 byte boundary, so we make this bigger than it needs to be
    *reinterpret_cast<size_t*>(pBuffer) = blockLength;

    pBuffer += sizeof(size_t);
    g_bufferPointer += blockLength + sizeof(size_t);

    return pBuffer;
}

// revert the very last allocation
void RipFreeLast(void* apBlock)
{
    uint8_t* block = static_cast<uint8_t*>(apBlock);

    const size_t tag = *reinterpret_cast<size_t*>(block - sizeof(size_t));
    g_bufferPointer -= tag + sizeof(size_t);
}

void RipFree(uint8_t* apBlock)
{
    // noop
}

#endif
