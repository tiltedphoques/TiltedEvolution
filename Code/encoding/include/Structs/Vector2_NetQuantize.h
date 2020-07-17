#pragma once

#include <Buffer.hpp>

using TiltedPhoques::Buffer;

struct Vector2_NetQuantize
{
    Vector2_NetQuantize() = default;
    ~Vector2_NetQuantize() = default;

    bool operator==(const Vector2_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Vector2_NetQuantize& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] uint32_t Pack() const noexcept;
    void Unpack(uint32_t aValue) noexcept;

    float X{}; 
    float Y{};
};
