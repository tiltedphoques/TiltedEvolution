#pragma once

#include <new>
#include <atomic>
#include <chrono>
#include <thread>

#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
    // 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
    constexpr std::size_t hardware_constructive_interference_size = 64;
    constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

template<class T, size_t Exponent = 18>
struct alignas(hardware_destructive_interference_size) fast_queue
{
    struct read_lock
    {
        read_lock(size_t cookie, fast_queue& instance)
            : m_cookie(cookie)
            , m_instance(instance)
        {}

        ~read_lock()
        {
            m_instance.read_release(m_cookie);
        }

    protected:

        size_t m_cookie;
        fast_queue& m_instance;
    };

    struct write_lock
    {
        write_lock(size_t cookie, fast_queue& instance)
            : m_cookie(cookie)
            , m_instance(instance)
        {}

        ~write_lock()
        {
            m_instance.write_release(m_cookie);
        }

    protected:

        size_t m_cookie;
        fast_queue& m_instance;
    };

    write_lock write_acquire(T*& data);
    read_lock read_acquire(T*& data);
    bool empty() const;

protected:

    friend read_lock;
    friend write_lock;

    void write_release(size_t cookie);
    void read_release(size_t cookie);

private:

    enum
    {
        kSize = 1 << Exponent,
        kMask = kSize - 1
    };

    size_t index(size_t i)
    {
        return i & kMask;
    }

    size_t round(size_t i)
    {
        return i >> Exponent;
    }

    struct alignas(hardware_destructive_interference_size) entry
    {
        std::atomic<size_t> state{ 0 };
        T data;
    };

    alignas(hardware_destructive_interference_size) std::atomic<size_t> m_last{ 0 };
    alignas(hardware_destructive_interference_size) std::atomic<size_t> m_first{ 0 };
    entry m_buffer[kSize];
};

template<class T, size_t Exponent>
typename fast_queue<T, Exponent>::write_lock fast_queue<T, Exponent>::write_acquire(T*& data)
{
    auto loc = m_last.fetch_add(1);
    auto& entry = m_buffer[index(loc)];
    const auto rnd = round(loc) << 1;
    while (rnd != entry.state.load(std::memory_order_acquire))
        std::this_thread::yield();

    data = &entry.data;

    return write_lock(loc, *this);
}

template<class T, size_t Exponent>
inline void fast_queue<T, Exponent>::write_release(size_t cookie)
{
    auto& entry = m_buffer[index(cookie)];
    const auto rnd = round(cookie) << 1;

    entry.state.store(rnd + 1, std::memory_order_release);
}

template<class T, size_t Exponent>
typename fast_queue<T, Exponent>::read_lock fast_queue<T, Exponent>::read_acquire(T*& data)
{
    auto loc = m_first.fetch_add(1);
    auto& entry = m_buffer[index(loc)];
    const auto rnd = round(loc) << 1;
    while (rnd + 1 != entry.state.load(std::memory_order_acquire))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    data = &entry.data;

    return read_lock(loc, *this);
}

template<class T, size_t Exponent>
inline void fast_queue<T, Exponent>::read_release(size_t cookie)
{
    auto& entry = m_buffer[index(cookie)];
    const auto rnd = round(cookie) << 1;

    entry.state.store(rnd + 2, std::memory_order_release);
}

template<class T, size_t Exponent>
inline bool fast_queue<T, Exponent>::empty() const
{
    return m_first.load(std::memory_order_relaxed) == m_last.load(std::memory_order_relaxed);
}
