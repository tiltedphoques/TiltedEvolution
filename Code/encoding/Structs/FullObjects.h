#pragma once

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Vector;
using TiltedPhoques::Buffer;

struct FullObjects
{
    Vector<unsigned char> Data{};

    FullObjects() = default;
    ~FullObjects() = default;

    bool operator==(const FullObjects& acRhs) const noexcept;
    bool operator!=(const FullObjects& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
};
