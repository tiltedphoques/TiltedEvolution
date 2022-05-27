#pragma once

#include "BSTArray.h"

namespace creation
{
void* BSTArrayHeapAllocator::GetBuffer() const
{
    return m_pBuffer;
}

inline uint32_t BSTArrayHeapAllocator::GetBufferSize() const
{
    return m_uiBufferSize;
}

template <class T, class Allocator> T* BSTArray<T, Allocator>::operator[](uint32_t aIndex)
{
    //BASE_ASSERT(aIndex >= m_uiSize, "Invalid index %u|%u passed to operator[]", aIndex, m_uiSize);
    BASE_ASSERT(this->m_pBuffer, "Array doesn't have a buffer");

    return &reinterpret_cast<T*>(this->m_pBuffer)[aIndex];
}

template <class T, class Allocator> const T* BSTArray<T, Allocator>::GetAt(uint32_t aIndex) const
{
    //BASE_ASSERT(aIndex >= m_uiSize, "Invalid index %u|%u passed to GetAt()", aIndex, m_uiSize);
    BASE_ASSERT(this->m_pBuffer, "Array doesn't have a buffer");

    return &reinterpret_cast<T*>(this->m_pBuffer)[aIndex];
}

template <class T, class Allocator> T* BSTArray<T, Allocator>::GetAt(uint32_t aIndex)
{
    //BASE_ASSERT(aIndex >= m_uiSize, "Invalid index %u|%u passed to GetAt()", aIndex, m_uiSize);
    BASE_ASSERT(this->m_pBuffer, "Array doesn't have a buffer");

    return &reinterpret_cast<T*>(this->m_pBuffer)[aIndex];
}

template <class T, class Allocator> const T* BSTArray<T, Allocator>::GetFirst()
{
    BASE_ASSERT(m_uiSize, "Array is empty");
    BASE_ASSERT(this->m_pBuffer, "Array doesn't have a buffer");

    return reinterpret_cast<const T*>(this->m_pBuffer);
}

template <class T, class Allocator> const T* BSTArray<T, Allocator>::GetLast() const
{
    BASE_ASSERT(m_uiSize, "Array is empty");
    BASE_ASSERT(this->m_pBuffer, "Array doesn't have a buffer");

    return &const_cast<const T*>(this->m_pBuffer)[m_uiSize - 1];
}

template <class T, class Allocator> T* BSTArray<T, Allocator>::GetLast()
{
    BASE_ASSERT(m_uiSize, "Array is empty");
    BASE_ASSERT(this->m_pBuffer, "Array doesn't have a buffer");

    return &reinterpret_cast<T*>(this->m_pBuffer)[m_uiSize - 1];
}

template <class T, class Allocator> int64_t BSTArray<T, Allocator>::Add(const T* apValue)
{
    Functor_t functor(this);

    const auto index = AddUninitialized(functor, this->m_uiBufferSize, sizeof(T));
    if (index != -1)
    {
        T* instance = const_cast<T*>(GetAt(index));
        if (instance)
            *(instance) = *apValue;
    }

    return index;
}

template <class T, class Allocator> bool BSTArray<T, Allocator>::Insert(uint32_t aIndex, const T* apValue)
{
    Functor_t functor(this);

    auto isInserted = InsertUninitialized(functor, this->m_pBuffer, this->m_uiBufferSize, aIndex, sizeof(T));
    if (isInserted)
    {
        T* instance = const_cast<T*>(GetAt(aIndex));
        if (instance)
            *(instance) = *apValue;
    }
    return isInserted;
}

template <class T, class Allocator> void BSTArray<T, Allocator>::Remove(uint32_t aIndex, uint32_t aHowMany)
{
    //BASE_ASSERT(aIndex >= m_uiSize, "Invalid index");
    //BASE_ASSERT(aHowMany >= 0xFFFFFFFD, "Invalid remove count");
    //BASE_ASSERT(aIndex + aHowMany > m_uiSize, "Too many items to remove");

    if (aHowMany == m_uiSize)
    {
        //BSASSERT(!this->m_pBuffer && (this->m_uiBufferSize || m_uiSize), "Invalid array");

        for (int i = 0; i < m_uiSize; i++)
        {
            T* item = GetAt(i);
            item->~T();
        }

        m_uiSize = 0;
    }
    else
    {
        if (aHowMany)
        {
            for (auto i = 0; i < aHowMany; i++)
            {
                T* item = GetAt(aIndex + i);
                item->~T();
            }
        }

        MoveItems(this->m_pBuffer, aIndex, aIndex + aHowMany, m_uiSize - aIndex - aHowMany, sizeof(T));

        m_uiSize -= aHowMany;
    }
}

template <class T, class Allocator> void BSTArray<T, Allocator>::Clear(bool aFreeMemory)
{
    //BASE_ASSERT(!this->m_pBuffer && (this->m_uiBufferSize || m_uiSize), "Invalid array");

    if (this->m_pBuffer)
    {
        if (m_uiSize)
        {
            for (int i = 0; i < m_uiSize; i++)
            {
                T* item = GetAt(i);
                item->~T();
            }
        }

        if (aFreeMemory)
            this->Deallocate();

        m_uiSize = 0;
    }
}

template <class T, class Allocator> int64_t BSTArray<T, Allocator>::Find(const T& arValue)
{
    if (!m_uiSize)
        return -1;

    for (size_t i = 0; i < m_uiSize; i++)
    {
        T* item = GetAt(i);
        if (*item == arValue)
            return i;
    }

    return -1;
}

template <class T, class Allocator> bool BSTArray<T, Allocator>::IsInArray(const T& arValue)
{
    return Find(arValue) != -1;
}
} // namespace creation
