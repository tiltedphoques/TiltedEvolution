// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <string>

namespace console
{
// Adapted from https://www.oreilly.com/library/view/c-cookbook/0596007612/ch04s08.html
class StringTokenizer
{
  public:
    StringTokenizer(const std::string& acInput, const char* acDelim = nullptr);

    size_t CountTokens();
    void GetNext(std::string& s);

    bool HasMore()
    {
        return (m_begin != m_end);
    }
  private:
    std::string m_delim;
    std::string m_string;
    int m_count;
    int m_begin;
    int m_end;
};
} // namespace console
