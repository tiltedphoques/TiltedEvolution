#pragma once

#include <cstdint>
#include <istream>
#include <ostream>
#include "Buffer.hpp"
#include "AnimationData.h"

struct AnimationVariables
{
    uint64_t Booleans{ 0 };
    std::array<uint32_t, AnimationData::kIntegerCount> Integers{};
    std::array<float, AnimationData::kFloatCount> Floats{};

    bool operator==(const AnimationVariables& acRhs) const noexcept;
    bool operator!=(const AnimationVariables& acRhs) const noexcept;

    void Load(std::istream&);
    void Save(std::ostream&) const;

    void GenerateDiff(const AnimationVariables& aPrevious, TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void ApplyDiff(TiltedPhoques::Buffer::Reader& aReader) noexcept;
};
