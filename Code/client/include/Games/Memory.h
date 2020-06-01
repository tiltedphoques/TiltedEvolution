#pragma once

struct Memory
{
    [[nodiscard]] static void* Allocate(size_t aSize) noexcept;
    static void Free(void* apData) noexcept;

    template<class T>
    static T* Allocate() noexcept
    {
        auto pMemory = static_cast<T*>(Allocate(sizeof(T)));
        return pMemory;
    }

    template<class T>
    static T* New() noexcept
    {
        return new (Allocate<T>()) T;
    }

    template<class T, class... TArgs>
    static T* New(TArgs... args) noexcept
    {
        auto pMemory = static_cast<T*>(Allocate(sizeof(T)));
        return new (Allocate<T>()) T(std::forward<TArgs>(args)...);
    }

    template<class T>
    static T* NewArray(const size_t aCount) noexcept
    {
        auto pMemory = static_cast<T*>(Allocate(sizeof(T) * aCount));
        for (size_t i = 0; i < aCount; ++i)
        {
            new (pMemory[i]) T;
        }

        return pMemory;
    }

    template<class T>
    static void Delete(T* apMemory) noexcept
    {
        assert(apMemory != nullptr);

        Free(apMemory);
    }
};
