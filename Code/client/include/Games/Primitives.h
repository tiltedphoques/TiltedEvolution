#pragma once

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
};

#if TP_PLATFORM_64
static_assert(offsetof(GameArray<int>, length) == 0x10);
static_assert(sizeof(GameArray<int>) == 0x18);
#else
static_assert(offsetof(GameArray<int>, length) == 0x8);
#endif

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

struct NiPoint3
{
    float x;
    float y;
    float z;

    float* AsArray()
    {
        return &x;
    }
};

struct NiPoint2
{
    float x;
    float y;

    float* AsArray()
    {
        return &x;
    }
};

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
        std::swap(m_pPointer, acRhs.m_pPointer);

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
