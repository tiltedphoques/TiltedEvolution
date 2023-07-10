// Copyright (C) 2022 Vincent Hengel.
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>

namespace Resources
{
// Semantic version class that can use up to 21 bits to represent each version number (major, minor, patch)
// 21 bits = 2097151 = 2^21 - 1
// 3 * 21 bits = 63 bits used for all three version numbers
// the empty bit is used to double the range of each version number from 2097151 to 4194302
// if it is set, every number starts at 2097151 instead of 0, but we only store the delta of (max) 2097151
struct SemanticVersion
{
    using underlying_type = uint64_t;

    static constexpr uint32_t k21BitMax = 2097151;
    static constexpr uint32_t k42BitMax = 4194302;

    SemanticVersion() = default;
    SemanticVersion(const underlying_type aVersionBits) noexcept
        : m_versionBits(aVersionBits)
    {
    }
    SemanticVersion(uint32_t aMajor, uint32_t aMinor, uint32_t aPatch) noexcept { From(aMajor, aMinor, aPatch); }
    SemanticVersion(const char* apVersion) noexcept { FromString(apVersion); }

    bool From(uint32_t major, uint32_t minor, uint32_t patch) noexcept
    {
        if (major > k42BitMax || minor > k42BitMax || patch > k42BitMax)
            return false;

        bool dirty = false;
        m_versionBits = 0;

        uint32_t vers[3] = {major, minor, patch};
        for (auto i = 0; i < 3; i++)
            if (vers[i] > k21BitMax)
                dirty = true;

        if (dirty)
        {
            m_versionBits |= 1ULL << 63;
            for (auto i = 0; i < 3; i++)
                vers[i] -= k21BitMax;
        }

        m_versionBits |= (uint64_t(vers[0]) << 42) + (uint64_t(vers[1]) << 21) + vers[2];
        return true;
    }

    bool FromString(const std::string_view acVersion) noexcept
    {
        uint64_t result = 0;
        uint32_t major = 0, minor = 0, patch = 0;
        int current = 0;

        // Parse the major, minor, and patch version numbers from the string
        for (size_t i = 0; i < acVersion.size(); i++)
        {
            if (acVersion[i] == '.')
            {
                current++;
                continue;
            }

            if (current == 0)
                major = major * 10 + (acVersion[i] - '0');
            else if (current == 1)
                minor = minor * 10 + (acVersion[i] - '0');
            else if (current == 2)
                patch = patch * 10 + (acVersion[i] - '0');
            else
                break;
        }

        return From(major, minor, patch);
    }

    std::tuple<uint32_t, uint32_t, uint32_t> ToTuple() const noexcept
    {
        uint32_t major = (m_versionBits >> 42) & k21BitMax;
        uint32_t minor = (m_versionBits >> 21) & k21BitMax;
        uint32_t patch = m_versionBits & k21BitMax;

        if (m_versionBits & (1ULL << 63))
        {
            major += k21BitMax;
            minor += k21BitMax;
            patch += k21BitMax;
        }
        return {major, minor, patch};
    }

    std::string ToString() const
    {
        auto [major, minor, patch] = ToTuple();
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }

    const underlying_type GetVersionBits() const noexcept { return m_versionBits; }

    const uint32_t GetMajor() const noexcept { return (m_versionBits >> 42) & k21BitMax; }

    const uint32_t GetMinor() const noexcept { return (m_versionBits >> 21) & k21BitMax; }

    const uint32_t GetPatch() const noexcept { return m_versionBits & k21BitMax; }

    bool IsExtended() const noexcept { return m_versionBits & (1ULL << 63); }

    operator bool() const noexcept { return m_versionBits != 0; }

    bool operator==(const SemanticVersion& aOther) const noexcept { return m_versionBits == aOther.m_versionBits; }

    bool operator<(const SemanticVersion& aOther) const noexcept { return m_versionBits < aOther.m_versionBits; }

    bool operator>(const SemanticVersion& aOther) const noexcept { return m_versionBits > aOther.m_versionBits; }

private:
    underlying_type m_versionBits = 0;
};

inline std::string SemVerToString(const SemanticVersion& aVersion) noexcept
{
    return aVersion.ToString();
}
} // namespace Resources
