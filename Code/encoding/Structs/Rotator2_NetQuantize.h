#pragma once

#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Buffer;

struct Rotator2_NetQuantize
{
    Rotator2_NetQuantize() = default;
    ~Rotator2_NetQuantize() = default;

    bool operator==(const Rotator2_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Rotator2_NetQuantize& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] uint32_t Pack() const noexcept;
    void Unpack(uint32_t aValue) noexcept;

    float X{}; 
    float Y{};
};
