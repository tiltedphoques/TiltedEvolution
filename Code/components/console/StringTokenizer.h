// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <TiltedCore/Stl.hpp>

namespace Console
{
// Adapted from https://www.oreilly.com/library/view/c-cookbook/0596007612/ch04s08.html
class StringTokenizer
{
  public:
    StringTokenizer(TiltedPhoques::String acInput, const char* acpDelim = nullptr);

    size_t CountTokens() noexcept;
    void GetNext(TiltedPhoques::String& s);

    bool HasMore() const noexcept
    {
        return (m_begin != m_end);
    }
  private:
    TiltedPhoques::String m_delim;
    TiltedPhoques::String m_string;
    int m_count;
    TiltedPhoques::String::size_type m_begin;
    TiltedPhoques::String::size_type m_end;
};
} // namespace Console
