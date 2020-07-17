#include <Structs/Movement.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool Movement::operator==(const Movement& acRhs) const noexcept
{
    return Position == acRhs.Position &&
        Rotation == acRhs.Rotation;
}

bool Movement::operator!=(const Movement& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Movement::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Position.Serialize(aWriter);
    Rotation.Serialize(aWriter);
}

void Movement::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Position.Deserialize(aReader);
    Rotation.Deserialize(aReader);
}
