#pragma once

#include <cstdint>
#include "TiltedCore/Buffer.hpp"
#include "TiltedCore/Stl.hpp"

using TiltedPhoques::Vector;

struct AnimationVariables
{
    uint64_t Booleans1{ 0 };
    uint64_t Booleans2{ 0 };
    Vector<uint32_t> Integers{};
    Vector<float> Floats{};

    bool operator==(const AnimationVariables& acRhs) const noexcept;
    bool operator!=(const AnimationVariables& acRhs) const noexcept;

    void Load(std::istream&);
    void Save(std::ostream&) const;

    void GenerateDiff(const AnimationVariables& aPrevious, TiltedPhoques::Buffer::Writer& aWriter) const;
    void ApplyDiff(TiltedPhoques::Buffer::Reader& aReader);
};
