// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <cstdint>

#include <TiltedCore/Platform.hpp>
#include <TiltedCore/Filesystem.hpp>

namespace Resources
{
    // represent a semantic version in a single 32 bit integer
    class SemanticVersion 
    {
    public:
        SemanticVersion() = default;
        SemanticVersion(const uint32_t aVersionBits) noexcept;
        SemanticVersion(uint32_t aMajor, uint32_t aMinor, uint32_t aPatch) noexcept;
        SemanticVersion(const char* apVersion) noexcept;

        uint32_t GetMajor() const noexcept { return (m_versionbits >> 8) & 0xFF; }
        uint32_t GetMinor() const noexcept { return (m_versionbits >> 4) & 0xFF; }
        uint32_t GetPatch() const noexcept { return m_versionbits & 0xFF; }

        TiltedPhoques::String ToString() const noexcept;
        bool FromString(const char *apText) const noexcept;

        bool operator==(const SemanticVersion& acRhs) const noexcept;
        bool operator<(const SemanticVersion& acRhs) const noexcept;
        bool operator>(const SemanticVersion& acRhs) const noexcept;

        operator bool() const noexcept { return m_versionbits != 0; }

    private:
        uint32_t m_versionbits = 0;
    };

    TiltedPhoques::String SemVerToString(const SemanticVersion& acVersion) noexcept;
} // namespace Resources