#pragma once

#include <Games/Memory.h>

template <class T>
struct GameArray
{
    T* data;
    alignas(sizeof(void*)) uint32_t capacity; // Might be size_t
    alignas(sizeof(void*)) uint32_t length;   // Might be size_t

    GameArray() : data{ nullptr }, capacity{}, length{ 0 } {}
    ~GameArray() = default;

    T& operator[] (uint32_t aIndex) { return data[aIndex]; }
    const T& operator[] (uint32_t aIndex) const { return data[aIndex]; }

    void Resize(size_t aLength) noexcept;

    // Range for loop compatibility
    struct Iterator
    {
        Iterator(T* apEntry) : m_pEntry(apEntry) {}
        Iterator operator++() { ++m_pEntry; return *this; }
        bool operator!=(const Iterator& acRhs) const { return m_pEntry != acRhs.m_pEntry; }
        const T& operator*() const { return *m_pEntry; }
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

template <class T>
void GameArray<T>::Resize(size_t aLength) noexcept
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

static_assert(offsetof(GameArray<int>, length) == 0x10);
static_assert(sizeof(GameArray<int>) == 0x18);

template <class T>
struct BSTSmallArray
{
    int32_t capacity; // 0
    T** data;         // 8
    uint32_t size;    // 10

    T* Get(uint32_t aIndex)
    {
        auto pData = reinterpret_cast<T**>(&data);
        if (capacity >= 0)
            pData = data;

        return pData[aIndex];
    }
};

#if TP_PLATFORM_64
static_assert(offsetof(BSTSmallArray<int>, size) == 0x10);
static_assert(sizeof(BSTSmallArray<int>) == 0x18);
#else
static_assert(offsetof(BSTSmallArray<int>, size) == 0x8);
static_assert(sizeof(BSTSmallArray<int>) == 0xC);
#endif


template <class T>
struct GameList
{
    struct Entry
    {
        T* data;
        Entry* next;
    };

    Entry entry;

    inline bool Empty() const noexcept
    {
        return entry.data == nullptr;
    }

    // Range for loop compatibility
    struct Iterator 
    {
        Iterator(Entry* apEntry) : m_pEntry(apEntry) {}
        Iterator operator++() { m_pEntry = m_pEntry->next; return *this; }
        bool operator!=(const Iterator& acRhs) const { return m_pEntry != acRhs.m_pEntry; }
        T* operator*() const { return m_pEntry->data; }
    private:
        Entry* m_pEntry;
    };

    Iterator begin() 
    {
        return Iterator(&entry);
    }

    Iterator end()
    {
        return Iterator(nullptr);
    }
};

#if TP_PLATFORM_64
static_assert(offsetof(GameArray<int>, length) == 0x10);
static_assert(sizeof(GameArray<int>) == 0x18);
#else
static_assert(offsetof(GameArray<int>, length) == 0x8);
#endif

struct NiRefObject
{
    virtual ~NiRefObject();

    virtual void Destroy();

    void IncRef();
    void DecRef();
    bool Release();

    volatile long refCount;
    uint32_t padC;
};

#if TP_PLATFORM_64
static_assert(sizeof(NiRefObject) == 0x10);
#endif

struct BSHandleRefObject : NiRefObject
{
    void DecRefHandle()
    {
        if ((InterlockedDecrement(&refCount) & 0x3FF) == 0)
            Destroy();
    }
};

struct NiPoint3 : glm::vec3
{
    NiPoint3() : glm::vec3()
    {
    }
    NiPoint3(const glm::vec3& acRhs) : glm::vec3(acRhs)
    {
    }

    float* AsArray()
    {
        return &x;
    }
};

static_assert(sizeof(NiPoint3) == 12);
static_assert(offsetof(NiPoint3, x) == 0);
static_assert(offsetof(NiPoint3, y) == 4);
static_assert(offsetof(NiPoint3, z) == 8);

struct NiPoint2 : glm::vec2
{
    float* AsArray()
    {
        return &x;
    }
};

static_assert(sizeof(NiPoint2) == 8);
static_assert(offsetof(NiPoint2, x) == 0);
static_assert(offsetof(NiPoint2, y) == 4);

struct BSRecursiveLock
{
    volatile DWORD m_tid;
    volatile LONG m_counter;

    void Lock();
    void Unlock();
};

template<class T>
struct BSScopedLock
{
    BSScopedLock(T& aLock) : m_lock(aLock) { m_lock.Lock(); }
    ~BSScopedLock() { m_lock.Unlock(); }

private:
    T& m_lock;
};

template<class T>
struct GamePtr
{
    GamePtr() 
        : m_pPointer(nullptr) 
    {}

    GamePtr(T* apPointer) 
        : m_pPointer(apPointer) 
    { 
        if (m_pPointer)
            m_pPointer->handleRefObject.IncRef();
    }

    GamePtr(const GamePtr<T>& acRhs)
        : m_pPointer(acRhs.m_pPointer)
    {
        if (m_pPointer)
            m_pPointer->handleRefObject.IncRef();
    }

    GamePtr(GamePtr<T>&& aRhs)
    {
        std::swap(m_pPointer, aRhs.m_pPointer);
    }

    GamePtr& operator=(const GamePtr<T>& acRhs)
    {
        Release();

        m_pPointer = acRhs.m_pPointer;

        if(m_pPointer)
            m_pPointer->handleRefObject.IncRef();

        return *this;
    }

    GamePtr& operator=(T* apPointer)
    {
        Release();

        m_pPointer = apPointer;

        if (m_pPointer)
            m_pPointer->handleRefObject.IncRef();

        return *this;
    }

    GamePtr& operator=(GamePtr<T>&& aRhs)
    {
        std::swap(m_pPointer, aRhs.m_pPointer);

        return *this;
    }

    T* operator->()
    {
        return m_pPointer;
    }

    T* operator*()
    {
        return m_pPointer;
    }

    operator T* ()
    {
        return m_pPointer;
    }

    ~GamePtr()
    { 
        Release();
    }

    void Release()
    {
        if (m_pPointer)
            m_pPointer->handleRefObject.DecRefHandle();

        m_pPointer = nullptr;
    }

private:

    T* m_pPointer;
};

struct alignas(sizeof(void*)) BSIntrusiveRefCounted
{
    volatile int32_t refCount;
};

static_assert(sizeof(BSIntrusiveRefCounted) == sizeof(void*));

enum ETiltedFlags : uint8_t
{
    kRemote = 1 << 0
};

template <class Target, class Source>
Target* niptr_cast(const Source& acSrc)
{
    return static_cast<Target*>(acSrc.object);
}

template<uint32_t FREE_LIST_BITS = 21, uint32_t AGE_SHIFT = 5>
struct BSUntypedPointerHandle
{
    uint32_t iBits = 0;
};

template<class T, class THandle = BSUntypedPointerHandle<>>
struct BSPointerHandle
{
    BSPointerHandle() = default;

    operator bool() const
    {
        return handle.iBits != 0;
    }

    THandle handle{};
};

struct BSReadWriteLock
{
    uint32_t uiWriterThread;
    volatile uint32_t uiLock;
};
