// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

// abi stable span
template <typename T> class PluginSlice
{
  public:
    explicit constexpr PluginSlice(const T* ptr, size_t len) : m_pData(ptr), m_size(len)
    {
    }
    template <size_t N> constexpr PluginSlice(T (&a)[N]) noexcept : PluginSlice(a, N)
    {
    }

    constexpr T* data() const
    {
        return m_pData;
    }

  private:
    T* m_pData;
    size_t m_size;
};
using PluginStringView = PluginSlice<const char>;
