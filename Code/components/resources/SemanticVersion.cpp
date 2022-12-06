// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "SemanticVersion.h"
#include <base/Check.h>

namespace Resources
{
    SemanticVersion::SemanticVersion(uint32_t aVersionBits) noexcept
        : m_versionbits(aVersionBits)
    {
        // TODO: check if the version is valid
    }

    SemanticVersion::SemanticVersion(uint32_t aMajor, uint32_t aMinor, uint32_t aPatch) noexcept
        : m_versionbits((aMinor << 16) | (aMajor << 8) | aPatch)
    {
    }

    SemanticVersion::SemanticVersion(const char* apVersion) noexcept
    {
        BASE_ASSERT(FromString(apVersion), "Invalid version string");
    }

// The function takes a string containing a version number in the format X.X.X (where each X is a single digit) and
// converts it to a 32-bit integer by encoding each digit in 4 bits, with the most significant digit being stored in the
// highest-order 4 bits of the integer. The periods are ignored. For example, the string "1.2.3" would be converted to
// the integer value 0x123.
    TiltedPhoques::String SemanticVersion::ToString() const noexcept
    {
        TiltedPhoques::String result;
        for (int i = 2; i >= 0; --i)
        {
            if (i < 2)
            {
                result += '.';
            }
            result += '0' + ((m_versionbits >> (i * 4)) & 0xF);
        }
        return result;
    }

// For each digit, we check if it is the least significant digit, and if it is not, we appends a period (.) to
// the string. Then, the code appends the character representation of the digit (by adding '0' to the numeric value of
// the digit) to the string.
/*
uint32_t version = 0;
    uint32_t shift = 0;
    int period_count = 0;

    // Skip leading whitespace
    while (*apText && std::isspace(*apText))
    {
        ++apText;
    }

    // Iterate over the string and extract the numeric components
    while (*apText)
    {
        // Check for non-numeric characters
        if (!std::isdigit(*apText))
        {
            // If the character is a period, increment the period count
            if (*apText == '.')
            {
                ++period_count;

                // If we've found more than three periods, return false
                if (period_count > 3)
                {
                    return false;
                }

                // Shift the bits to the left by 8
                shift += 8;
            }
            else
            {
                // If the character is not a period, return false
                return false;
            }
        }
        else
        {
            // If the character is a digit, shift the bits to the left by 4
            version |= (*apText - '0') << shift;
            shift += 4;
        }
        ++apText;
    }

    // Skip trailing whitespace
    while (*apText && std::isspace(*apText))
    {
        ++apText;
    }

    // If the string contains only numeric characters and at most three periods, return true
    if (*apText == '\0')
    {
        return true;
    }
    else
    {
        return false;
    }
*/
    bool SemanticVersion::FromString(const char* apText) const noexcept
    {
        uint32_t version = 0;
        uint32_t shift = 0;
        while (*apText)
        {
            if (*apText == '.')
            {
                shift += 8;
            }
            else
            {
                version |= (*apText - '0') << shift;
                shift += 4;
            }
            ++apText;
        }
        return version;
    }

    bool SemanticVersion::operator==(const SemanticVersion& acRhs) const noexcept
    {
        return m_versionbits == acRhs.m_versionbits;
    }

    bool SemanticVersion::operator<(const SemanticVersion& acRhs) const noexcept
    {
        return m_versionbits < acRhs.m_versionbits;
    }

    bool SemanticVersion::operator>(const SemanticVersion& acRhs) const noexcept
    {
        return m_versionbits > acRhs.m_versionbits;
    }

    TiltedPhoques::String SemVerToString(const SemanticVersion& acVersion) noexcept
    {
        return acVersion.ToString();
    }
}