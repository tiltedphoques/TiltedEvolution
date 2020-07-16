#include <Structs/Vector3_NetQuantize.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool Vector3_NetQuantize::operator==(const Vector3_NetQuantize& acRhs) const noexcept
{
    return Pack() == acRhs.Pack();
}

bool Vector3_NetQuantize::operator!=(const Vector3_NetQuantize& acRhs) const noexcept
{
    return !this->operator==(acRhs);
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

    X = xSign ? -xValue : xValue;
    Y = ySign ? -yValue : yValue;
    Z = zSign ? -zValue : zValue;
}

uint64_t Vector3_NetQuantize::Pack() const noexcept
{
    uint64_t data = 0;

    int32_t x = static_cast<int32_t>(X);
    int32_t y = static_cast<int32_t>(Y);
    int32_t z = static_cast<int32_t>(Z);

    data |= (x < 0) ? 1 : 0;
    data |= (y < 0) ? 2 : 0;
    data |= (z < 0) ? 4 : 0;

    const uint64_t xVal = std::abs(x) & ((1 << 20) - 1);
    const uint64_t yVal = std::abs(y) & ((1 << 20) - 1);
    const uint64_t zVal = std::abs(z) & ((1 << 20) - 1);

    data |= xVal << 3;
    data |= yVal << 23;
    data |= zVal << 43;

    return data;
}
