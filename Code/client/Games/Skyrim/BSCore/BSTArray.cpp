
#include "BSCore/BSTArray.h"

namespace creation
{
bool BSTArrayHeapAllocator::Allocate(uint32_t auiMinSize, uint32_t auiSize)
{
    const uint32_t minSize = auiMinSize > 4 ? auiMinSize : 4;
    const size_t allocSize = static_cast<size_t>(auiSize) * minSize;

    if (m_pBuffer = reinterpret_cast<uint8_t*>(Memory::Allocate(allocSize)))
    {
        m_uiBufferSize = minSize;
    }

    return m_pBuffer;
}

void BSTArrayHeapAllocator::Deallocate()
{
    Memory::Free(static_cast<void*>(m_pBuffer));

    m_pBuffer = nullptr;
    m_uiBufferSize = 0;
}

bool BSTArrayHeapAllocator::Reallocate(uint32_t aiMinNewSizeInItems, uint32_t aiFrontCopyCount, uint32_t aiShiftCount,
                                       uint32_t aiBackCopyCount, uint32_t aiElemSize)
{
    uint32_t minSize = aiMinNewSizeInItems;
    if (2 * m_uiBufferSize > aiMinNewSizeInItems)
        minSize = 2 * m_uiBufferSize;

    if (!m_pBuffer)
        return Allocate(minSize, aiElemSize);

    const size_t allocSize = static_cast<size_t>(aiElemSize) * minSize;
    uint8_t* newBuffer = reinterpret_cast<uint8_t*>(Memory::Allocate(allocSize));

    if (newBuffer)
    {
        if (aiFrontCopyCount)
            std::memmove(newBuffer, m_pBuffer, aiElemSize * aiFrontCopyCount);
        if (aiBackCopyCount)
            std::memmove(&newBuffer[aiElemSize * (aiFrontCopyCount * aiShiftCount)],
                         &const_cast<const char*>(static_cast<char*>(m_pBuffer))[aiElemSize * aiFrontCopyCount],
                         aiElemSize * aiBackCopyCount);

        delete[] m_pBuffer;
        m_pBuffer = newBuffer;
        m_uiBufferSize = minSize;
    }

    return m_pBuffer;
}

bool BSTArrayBase::SetAllocSize(IAllocatorFunctor* aFunctor, uint32_t auiAllocSize, uint32_t auiNewAllocSize,
                                uint32_t auiElemSize)
{
    const uint32_t minSize = auiNewAllocSize < m_uiSize ? m_uiSize : auiNewAllocSize;
    if (minSize == auiAllocSize)
        return true;

    if (!minSize)
    {
        aFunctor->Deallocate();
        return false;
    }

    if (minSize <= auiAllocSize)
        return true;

    if (auiAllocSize)
        return aFunctor->Reallocate(minSize, m_uiSize, 0, 0, auiElemSize);
    else
        return aFunctor->Allocate(minSize, auiElemSize);
}

int32_t BSTArrayBase::AddUninitialized(IAllocatorFunctor& arFunctor, uint32_t aAllocSize, uint32_t aElemSize)
{
    BASE_ASSERT(aAllocSize > m_uiSize, "Array overflow");

    if (m_uiSize < aAllocSize)
    {
        return m_uiSize++;
    }

    BASE_ASSERT(aAllocSize == m_uiSize, "Array overflow");

    if (!aAllocSize)
    {
        if (arFunctor.Allocate(1, aElemSize))
        {
            m_uiSize = 1;
            return 0;
        }
    }
    else if (arFunctor.Reallocate(m_uiSize + 1, m_uiSize, 0, 0, aElemSize))
    {
        return m_uiSize++;
    }

    return -1;
}

void BSTArrayBase::MoveItems(void* apBuffer, uint32_t aDestIndex, uint32_t aSrcIndex, uint32_t aCount,
                             uint32_t aElemSize)
{
    BASE_ASSERT(aCount < m_uiSize, "move item over/underflow");

    std::memmove(static_cast<char*>(apBuffer) + aDestIndex * aElemSize,
                 static_cast<char*>(apBuffer) + aElemSize * aSrcIndex, aElemSize * aCount);
}

bool BSTArrayBase::InsertUninitialized(IAllocatorFunctor& arFunctor, void* apBuffer, uint32_t aAllocSize,
                                       uint32_t aIndex, uint32_t aElemSize)
{
    BASE_ASSERT(aIndex < m_uiSize, "Array overflow");
    BASE_ASSERT(aAllocSize > m_uiSize, "Array overflow");

    if (aIndex == m_uiSize)
        return AddUninitialized(arFunctor, aAllocSize, aElemSize) != -1;

    bool result;
    if (m_uiSize == aAllocSize)
    {
        result = arFunctor.Reallocate(m_uiSize + 1, aIndex, 1u, m_uiSize - aIndex, aElemSize);
        if (!result)
            return result;
    }
    else
    {
        MoveItems(apBuffer, aIndex + 1, aIndex, m_uiSize - aIndex, aElemSize);
        result = true;
    }

    ++m_uiSize;
    return result;
}
} // namespace creation
