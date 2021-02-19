#include <Structs/Rotator2_NetQuantize.h>
#include <TiltedCore/Math.hpp>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

constexpr float cScalingFactory = float(0xFFFF) / (2.0f * float(TiltedPhoques::Pi));
constexpr float cReverseScalingFactory = 1.0f / cScalingFactory;

bool Rotator2_NetQuantize::operator==(const Rotator2_NetQuantize& acRhs) const noexcept
{
    return Pack() == acRhs.Pack();
}

bool Rotator2_NetQuantize::operator!=(const Rotator2_NetQuantize& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Rotator2_NetQuantize::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Pack(), 32);
}

void Rotator2_NetQuantize::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t data = 0;
    aReader.ReadBits(data, 32);

    Unpack(data & 0xFFFFFFFF);
}

void Rotator2_NetQuantize::Unpack(uint32_t aValue) noexcept
{
    auto xValue = static_cast<float>(aValue & 0xFFFF);
    auto yValue = static_cast<float>((aValue >> 16) & 0xFFFF);

    X = xValue * cReverseScalingFactory;
    Y = yValue * cReverseScalingFactory;
}

uint32_t Rotator2_NetQuantize::Pack() const noexcept
{
    uint32_t data = 0;

    auto WrapAngle = [](float angle)
    {
        angle = TiltedPhoques::Mod(angle, 2.f * float(TiltedPhoques::Pi));
        if (angle < 0.f)
        {
            angle += 2.f * float(TiltedPhoques::Pi);
        }

        return angle;
    };

    uint32_t x = static_cast<uint32_t>(WrapAngle(X) * cScalingFactory) & 0xFFFF;
    uint32_t y = static_cast<uint32_t>(WrapAngle(Y) * cScalingFactory) & 0xFFFF;

    data |= x;
    data |= y << 16;

    return data;
}
