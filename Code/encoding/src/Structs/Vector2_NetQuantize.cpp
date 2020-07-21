#include <Structs/Vector2_NetQuantize.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool Vector2_NetQuantize::operator==(const Vector2_NetQuantize& acRhs) const noexcept
{
    return Pack() == acRhs.Pack();
}

bool Vector2_NetQuantize::operator!=(const Vector2_NetQuantize& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Vector2_NetQuantize::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Pack(), 32);
}

void Vector2_NetQuantize::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t data;
    aReader.ReadBits(data, 32);

    Unpack(data & 0xFFFFFFFF);
}

void Vector2_NetQuantize::Unpack(uint32_t aValue) noexcept
{
    int32_t xSign = (aValue & 1) != 0;
    int32_t ySign = (aValue & 2) != 0;

    auto xValue = static_cast<float>((aValue >> 2) & ((1 << 15) - 1));
    auto yValue = static_cast<float>((aValue >> 17) & ((1 << 15) - 1));

    X = xSign ? -xValue : xValue;
    Y = ySign ? -yValue : yValue;
}

uint32_t Vector2_NetQuantize::Pack() const noexcept
{
    uint32_t data = 0;

    int32_t x = static_cast<int32_t>(X);
    int32_t y = static_cast<int32_t>(Y);

    data |= (x < 0) ? 1 : 0;
    data |= (y < 0) ? 2 : 0;

    const uint64_t xVal = std::abs(x) & ((1 << 15) - 1);
    const uint64_t yVal = std::abs(y) & ((1 << 15) - 1);

    data |= xVal << 2;
    data |= yVal << 17;

    return data;
}
