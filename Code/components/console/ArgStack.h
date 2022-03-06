// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <any>
#include <TiltedCore/Stl.hpp>

namespace Console
{
// Optimized for small size.
class ArgStack
{
  public:
    // Default ctor for queue emplacement
    ArgStack() = default;

    explicit ArgStack(size_t acCount)
    {
        m_pArgs = TiltedPhoques::MakeUnique<std::any[]>(acCount);
    }

    // Move operator for em placing within queue
    ArgStack& operator=(ArgStack&& other) noexcept
    {
        m_pArgs = std::move(other.m_pArgs);
        m_Count = other.m_Count;
        return *this;
    }

    template <typename T> inline void Push(T&& x)
    {
        m_pArgs[m_Count] = x;
        m_Count++;
    }

    template <typename T> T Pop()
    {
        auto v = std::any_cast<T>(m_pArgs[m_Count]);
        m_Count++;
        return v;
    }

    // This must be called before calling Pop() and is done
    // for optimization purposes.
    void ResetCounter()
    {
        m_Count = 0;
    }

  private:
    TiltedPhoques::UniquePtr<std::any[]> m_pArgs = nullptr;
    size_t m_Count = 0;
};
} // namespace Console
