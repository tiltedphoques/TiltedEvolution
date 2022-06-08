#pragma once

using TiltedPhoques::Buffer;

struct Rotator2_NetQuantize : glm::vec2
{
    Rotator2_NetQuantize() = default;
    ~Rotator2_NetQuantize() = default;
    Rotator2_NetQuantize(const Rotator2_NetQuantize&) = default;
    Rotator2_NetQuantize(Rotator2_NetQuantize&&) = default;
    Rotator2_NetQuantize& operator=(const Rotator2_NetQuantize&) = default;
    Rotator2_NetQuantize& operator=(Rotator2_NetQuantize&&) = default;

    bool operator==(const Rotator2_NetQuantize& acRhs) const noexcept;
    bool operator!=(const Rotator2_NetQuantize& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    [[nodiscard]] uint32_t Pack() const noexcept;
    void Unpack(uint32_t aValue) noexcept;
};
