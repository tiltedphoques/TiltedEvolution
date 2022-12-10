// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

namespace PluginAPI
{
// abi stable span that prevents accessing elements out of bounds
template <typename T> class Slice
{
  public:
    explicit inline constexpr Slice(const T* ptr, size_t len) : m_pData(ptr), m_size(len)
    {
    }
    template <size_t N> inline constexpr Slice(T (&a)[N]) noexcept : Slice(a, N)
    {
    }

    constexpr inline const T* data() const
    {
        return m_pData;
    }

  private:
    const T* m_pData;
    size_t m_size;
};
using StringRef = Slice<const char>;
}
