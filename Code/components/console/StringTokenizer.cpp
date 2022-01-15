// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <console/StringTokenizer.h>

namespace console
{
StringTokenizer::StringTokenizer(const std::string& acInput, const char* acDelim)
    : m_string(acInput), m_count(-1), m_begin(0), m_end(0)
{

    if (!acDelim)
        m_delim = " \f\n\r\t\v"; // default to whitespace
    else
        m_delim = acDelim;

    // Point to the first token
    m_begin = m_string.find_first_not_of(m_delim);
    m_end = m_string.find_first_of(m_delim, m_begin);
}

size_t StringTokenizer::CountTokens()
{
    if (m_count >= 0) // return if we've already counted
        return (m_count);

    std::string::size_type n = 0;
    std::string::size_type i = 0;

    for (;;)
    {
        // advance to the first token
        if ((i = m_string.find_first_not_of(m_delim, i)) == std::string::npos)
            break;
        // advance to the next delimiter
        i = m_string.find_first_of(m_delim, i + 1);
        n++;
        if (i == std::string::npos)
            break;
    }
    return (m_count = n);
}

void StringTokenizer::GetNext(std::string& s)
{
    if (m_begin != std::string::npos && m_end != std::string::npos)
    {
        s = m_string.substr(m_begin, m_end - m_begin);
        m_begin = m_string.find_first_not_of(m_delim, m_end);
        m_end = m_string.find_first_of(m_delim, m_begin);
    }
    else if (m_begin != std::string::npos && m_end == std::string::npos)
    {
        s = m_string.substr(m_begin, m_string.length() - m_begin);
        m_begin = m_string.find_first_not_of(m_delim, m_end);
    }
}
} // namespace console
