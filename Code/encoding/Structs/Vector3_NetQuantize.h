#pragma once

using TiltedPhoques::Buffer;

//! Network optimized 3d vector
struct Vector3_NetQuantize : glm::vec3
{
    Vector3_NetQuantize() = default;
    ~Vector3_NetQuantize() = default;

    bool operator==(const Vector3_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Vector3_NetQuantize& acRhs) const noexcept;

    Vector3_NetQuantize& operator=(const glm::vec3& acRhs) noexcept;

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
     * Packs the vector into a 64 bits representation of the network vector
     */
    [[nodiscard]] uint64_t Pack() const noexcept;
    /**
     * Unpack a 64 bits representation of a network vector
     * @param aValue The 64bits representation of a vector.
     */
    void Unpack(uint64_t aValue) noexcept;
};
