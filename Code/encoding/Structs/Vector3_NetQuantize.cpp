#include <Structs/Vector3_NetQuantize.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool Vector3_NetQuantize::operator==(const Vector3_NetQuantize& acRhs) const noexcept
{
    return Pack() == acRhs.Pack();
}

bool Vector3_NetQuantize::operator!=(const Vector3_NetQuantize& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

Vector3_NetQuantize& Vector3_NetQuantize::operator=(const glm::vec3& acRhs) noexcept
{
    glm::vec3::operator=(acRhs);
    return *this;
}

void Vector3_NetQuantize::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Pack(), 64);
}

void Vector3_NetQuantize::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t data;
    aReader.ReadBits(data, 64);

    Unpack(data);
}

void Vector3_NetQuantize::Unpack(uint64_t aValue) noexcept
{
    int32_t xSign = (aValue & 1) != 0;
    int32_t ySign = (aValue & 2) != 0;
    int32_t zSign = (aValue & 4) != 0;

    auto xValue = static_cast<float>((aValue >> 3) & ((1 << 20) - 1));
    auto yValue = static_cast<float>((aValue >> 23) & ((1 << 20) - 1));
    auto zValue = static_cast<float>((aValue >> 43) & ((1 << 20) - 1));

    x = xSign ? -xValue : xValue;
    y = ySign ? -yValue : yValue;
    z = zSign ? -zValue : zValue;
}

uint64_t Vector3_NetQuantize::Pack() const noexcept
{
    uint64_t data = 0;

    int32_t ix = static_cast<int32_t>(x);
    int32_t iy = static_cast<int32_t>(y);
    int32_t iz = static_cast<int32_t>(z);

    data |= (ix < 0) ? 1 : 0;
    data |= (iy < 0) ? 2 : 0;
    data |= (iz < 0) ? 4 : 0;

    const uint64_t xVal = std::abs(ix) & ((1 << 20) - 1);
    const uint64_t yVal = std::abs(iy) & ((1 << 20) - 1);
    const uint64_t zVal = std::abs(iz) & ((1 << 20) - 1);

    data |= xVal << 3;
    data |= yVal << 23;
    data |= zVal << 43;

    return data;
}
