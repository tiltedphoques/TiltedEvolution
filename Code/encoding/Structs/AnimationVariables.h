#pragma once

#include <cstdint>

using TiltedPhoques::Vector;

struct AnimationVariables
{
    Vector<bool> Booleans{};
    Vector<uint32_t> Integers{};
    Vector<float> Floats{};

    bool operator==(const AnimationVariables& acRhs) const noexcept;
    bool operator!=(const AnimationVariables& acRhs) const noexcept;

    void Load(std::istream&);
    void Save(std::ostream&) const;

    void GenerateDiff(const AnimationVariables& aPrevious, TiltedPhoques::Buffer::Writer& aWriter) const;
    void ApplyDiff(TiltedPhoques::Buffer::Reader& aReader);
    void VectorBool_to_String(const Vector<bool>& bools, TiltedPhoques::String& chars) const;
    void String_to_VectorBool(const TiltedPhoques::String& chars, Vector<bool>& bools);
};
