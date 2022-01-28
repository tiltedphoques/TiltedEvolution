#pragma once

#include <Games/Memory.h>

template <typename T> struct NiTObjectArray
{
    T* data;
    uint16_t length;
    uint16_t unk1;
    uint16_t size;
    uint16_t capacity;

    NiTObjectArray() : data{nullptr}, capacity{}, length{0}, unk1{0}, size{0}
    {
    }

    virtual ~NiTObjectArray() = default;

    T& operator[](uint32_t aIndex)
    {
        return data[aIndex];
    }
    const T& operator[](uint32_t aIndex) const
    {
        return data[aIndex];
    }

    // Range for loop compatibility
    struct Iterator
    {
        Iterator(T* apEntry) : m_pEntry(apEntry)
        {
        }
        Iterator operator++()
        {
            ++m_pEntry;
            return *this;
        }
        bool operator!=(const Iterator& acRhs) const
        {
            return m_pEntry != acRhs.m_pEntry;
        }
        const T& operator*() const
        {
            return *m_pEntry;
        }

      private:
        T* m_pEntry;
    };

    Iterator begin()
    {
        return Iterator(&data[0]);
    }

    Iterator end()
    {
        return Iterator(&data[length]);
    }

    inline bool Empty() const noexcept
    {
        return length == 0;
    }
};

static_assert(sizeof(NiTObjectArray<uintptr_t>) == 24);
