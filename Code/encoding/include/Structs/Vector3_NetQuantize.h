#pragma once

#include <Buffer.hpp>

using TiltedPhoques::Buffer;

struct Vector3_NetQuantize
{
    Vector3_NetQuantize() = default;
    ~Vector3_NetQuantize() = default;

    bool operator==(const Vector3_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Vector3_NetQuantize& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] uint64_t Pack() const noexcept;
    void Unpack(uint64_t aValue) noexcept;

    float X{}; 
    float Y{};
    float Z{};
};
