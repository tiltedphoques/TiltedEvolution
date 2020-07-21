#pragma once

#include <Buffer.hpp>
#include <Vector3.hpp>

using TiltedPhoques::Buffer;

struct Vector3_NetQuantize : TiltedPhoques::Vector3<float>
{
    Vector3_NetQuantize() = default;
    ~Vector3_NetQuantize() = default;

    bool operator==(const Vector3_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Vector3_NetQuantize& acRhs) const noexcept;

    Vector3_NetQuantize& operator=(const Vector3<float>& acRhs) noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] uint64_t Pack() const noexcept;
    void Unpack(uint64_t aValue) noexcept;
};
