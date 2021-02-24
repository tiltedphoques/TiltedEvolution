#pragma once

#include <TiltedCore/Buffer.hpp>
#include <glm/vec2.hpp>

using TiltedPhoques::Buffer;

struct Vector2_NetQuantize : glm::vec2
{
    Vector2_NetQuantize() = default;
    ~Vector2_NetQuantize() = default;

    bool operator==(const Vector2_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Vector2_NetQuantize& acRhs) const noexcept;

    Vector2_NetQuantize& operator=(const glm::vec2& acRhs) noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] uint32_t Pack() const noexcept;
    void Unpack(uint32_t aValue) noexcept;
};
