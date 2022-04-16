// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <string>

namespace Console
{
// Adapted from https://www.oreilly.com/library/view/c-cookbook/0596007612/ch04s08.html
class StringTokenizer
{
  public:
    StringTokenizer(std::string acInput, const char* acpDelim = nullptr);

    size_t CountTokens() noexcept;
    void GetNext(std::string& s);

    bool HasMore() const noexcept
    {
        return (m_begin != m_end);
    }
  private:
    std::string m_delim;
    std::string m_string;
    size_t m_count;
    std::string::size_type m_begin;
    std::string::size_type m_end;
};
} // namespace Console
