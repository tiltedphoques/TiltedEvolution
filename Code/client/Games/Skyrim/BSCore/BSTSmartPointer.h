#pragma once

#include "BSCore/BSIntrusiveRefCounted.h"

// Ported from CommonLibSSE (RE::BSTSmartPointer), trimmed down to what we use
template <class T> struct BSTSmartPointer
{
    BSTSmartPointer() noexcept = default;
    BSTSmartPointer(std::nullptr_t) noexcept {}

    // Acquires its own reference, the caller keeps theirs
    explicit BSTSmartPointer(T* apPointer) noexcept
        : m_pPointer(apPointer)
    {
        if (m_pPointer)
            m_pPointer->IncRef();
    }

    BSTSmartPointer(const BSTSmartPointer& acRhs) noexcept
        : m_pPointer(acRhs.m_pPointer)
    {
        if (m_pPointer)
            m_pPointer->IncRef();
    }

    BSTSmartPointer(BSTSmartPointer&& aRhs) noexcept
        : m_pPointer(std::exchange(aRhs.m_pPointer, nullptr))
    {
    }

    ~BSTSmartPointer()
    {
        // delete must dispatch to the game's scalar deleting dtor, which frees to the game heap.
        // Without a virtual dtor it would free through our CRT heap instead.
        static_assert(std::has_virtual_destructor_v<T>);

        if (m_pPointer && m_pPointer->DecRef() == 0)
            delete m_pPointer;
    }

    // Taking the argument by value covers both copy and move assignment
    BSTSmartPointer& operator=(BSTSmartPointer aRhs) noexcept
    {
        std::swap(m_pPointer, aRhs.m_pPointer);
        return *this;
    }

    [[nodiscard]] T* get() const noexcept { return m_pPointer; }
    [[nodiscard]] T* operator->() const noexcept { return m_pPointer; }
    [[nodiscard]] explicit operator bool() const noexcept { return m_pPointer != nullptr; }

private:
    T* m_pPointer{nullptr};
};

static_assert(sizeof(BSTSmartPointer<BSIntrusiveRefCounted>) == sizeof(void*));
