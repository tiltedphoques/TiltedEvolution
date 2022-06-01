#pragma once


using TiltedPhoques::Buffer;

//! Network optimized 2d vector
struct Vector2_NetQuantize : glm::vec2
{
    Vector2_NetQuantize() = default;
    ~Vector2_NetQuantize() = default;

    bool operator==(const Vector2_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Vector2_NetQuantize& acRhs) const noexcept;

    Vector2_NetQuantize& operator=(const glm::vec2& acRhs) noexcept;

    /**
     * Serialize to a buffer.
     * @param aWriter Writer wrapping the buffer.
     */
    void Serialize(Buffer::Writer& aWriter) const noexcept;
    /**
     * Deserialize from a buffer.
     * @param aReader Reader wrapping the buffer.
     */
    void Deserialize(Buffer::Reader& aReader) noexcept;
    /**
     * Packs the vector into a 32 bits representation of the network vector
     */
    [[nodiscard]] uint32_t Pack() const noexcept;
    /**
     * Unpack a 32 bits representation of a network vector
     * @param aValue The 64bits representation of a vector.
     */
    void Unpack(uint32_t aValue) noexcept;
};
