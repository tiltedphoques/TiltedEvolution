#pragma once

// TODO: needs to be done "properly"

template <class T> struct BSTArray
{
    T* data;
    alignas(sizeof(void*)) uint32_t capacity; // Might be size_t
    alignas(sizeof(void*)) uint32_t length;   // Might be size_t

    BSTArray()
        : data{nullptr}
        , capacity{}
        , length{0}
    {
    }
    ~BSTArray() = default;

    T& operator[](uint32_t aIndex) { return data[aIndex]; }
    const T& operator[](uint32_t aIndex) const { return data[aIndex]; }

    void Resize(size_t aLength) noexcept;

    // Range for loop compatibility
    struct Iterator
    {
        Iterator(T* apEntry)
            : m_pEntry(apEntry)
        {
        }
        Iterator operator++()
        {
            ++m_pEntry;
            return *this;
        }
        bool operator!=(const Iterator& acRhs) const { return m_pEntry != acRhs.m_pEntry; }
        const T& operator*() const { return *m_pEntry; }

    private:
        T* m_pEntry;
    };

    Iterator begin() { return Iterator(&data[0]); }

    Iterator end() { return Iterator(&data[length]); }

    inline bool Empty() const noexcept { return length == 0; }
};

template <class T> void BSTArray<T>::Resize(size_t aLength) noexcept
{
    if (capacity >= aLength)
    {
        length = aLength & 0xFFFFFFFF;
    }
    else
    {
        T* pNewData = reinterpret_cast<T*>(Memory::Allocate(sizeof(T) * aLength));
        std::copy(data, data + length, pNewData);
        Memory::Free(data);
        data = pNewData;
        capacity = length = aLength;
    }
}

static_assert(offsetof(BSTArray<int>, length) == 0x10);
static_assert(sizeof(BSTArray<int>) == 0x18);
