#pragma once

#include <utility>

// TODO: not lore accurate
template <class T> struct BSTSmartPointer
{
    BSTSmartPointer()
        : m_pPointer(nullptr)
    {
    }

    BSTSmartPointer(T* apPointer)
        : m_pPointer(apPointer)
    {
        if (m_pPointer)
            m_pPointer->IncRefCount();
    }

    BSTSmartPointer(const BSTSmartPointer<T>& acRhs)
        : m_pPointer(acRhs.m_pPointer)
    {
        if (m_pPointer)
            m_pPointer->IncRefCount();
    }

    BSTSmartPointer(BSTSmartPointer<T>&& aRhs) { std::swap(m_pPointer, aRhs.m_pPointer); }

    BSTSmartPointer& operator=(const BSTSmartPointer<T>& acRhs)
    {
        Release();

        m_pPointer = acRhs.m_pPointer;

        if (m_pPointer)
            m_pPointer->IncRefCount();

        return *this;
    }

    BSTSmartPointer& operator=(T* apPointer)
    {
        Release();

        m_pPointer = apPointer;

        if (m_pPointer)
            m_pPointer->IncRefCount();

        return *this;
    }

    BSTSmartPointer& operator=(BSTSmartPointer<T>&& aRhs)
    {
        std::swap(m_pPointer, aRhs.m_pPointer);

        return *this;
    }

    T* operator->() { return m_pPointer; }

    T* operator*() { return m_pPointer; }

    operator T*() { return m_pPointer; }

    ~BSTSmartPointer() { Release(); }

    void Release()
    {
        if (m_pPointer)
            m_pPointer->DecRefCount();

        m_pPointer = nullptr;
    }

private:
    T* m_pPointer;
};
