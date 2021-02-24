#pragma once

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Vector;
using TiltedPhoques::Buffer;

struct Scripts
{
    Vector<unsigned char> Data{};

    Scripts() = default;
    ~Scripts() = default;

    bool operator==(const Scripts& acRhs) const noexcept;
    bool operator!=(const Scripts& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
};
