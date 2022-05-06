// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <console/StringTokenizer.h>

namespace Console
{
StringTokenizer::StringTokenizer(TiltedPhoques::String acInput, const char* acpDelim)
    : m_string(std::move(acInput)), m_count(-1), m_begin(0), m_end(0)
{

    if (!acpDelim)
        m_delim = " \f\n\r\t\v"; // default to whitespace
    else
        m_delim = acpDelim;

    // Point to the first token
    m_begin = m_string.find_first_not_of(m_delim);
    m_end = m_string.find_first_of(m_delim, m_begin);
}

size_t StringTokenizer::CountTokens() noexcept
{
    if (m_count >= 0) // return if we've already counted
        return (m_count);

    TiltedPhoques::String::size_type n = 0;
    TiltedPhoques::String::size_type i = 0;

    for (;;)
    {
        // advance to the first token
        if ((i = m_string.find_first_not_of(m_delim, i)) == TiltedPhoques::String::npos)
            break;
        // advance to the next delimiter
        i = m_string.find_first_of(m_delim, i + 1);
        n++;
        if (i == TiltedPhoques::String::npos)
            break;
    }
    return (m_count = n);
}

void StringTokenizer::GetNext(TiltedPhoques::String& s)
{
    if (m_begin != TiltedPhoques::String::npos && m_end != TiltedPhoques::String::npos)
    {
        s = m_string.substr(m_begin, m_end - m_begin);
        m_begin = m_string.find_first_not_of(m_delim, m_end);
        m_end = m_string.find_first_of(m_delim, m_begin);
    }
    else if (m_begin != TiltedPhoques::String::npos && m_end == TiltedPhoques::String::npos)
    {
        s = m_string.substr(m_begin, m_string.length() - m_begin);
        m_begin = m_string.find_first_not_of(m_delim, m_end);
    }
}
} // namespace Console
