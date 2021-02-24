#pragma once

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Vector;
using TiltedPhoques::Buffer;

struct Objects
{
    Vector<unsigned char> Data{};

    Objects() = default;
    ~Objects() = default;

    [[nodiscard]] bool IsEmpty() const noexcept;
    bool operator==(const Objects& acRhs) const noexcept;
    bool operator!=(const Objects& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
};
