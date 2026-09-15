#include "RequestTimeSkip.h"

#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

void RequestTimeSkip::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, Hours);
}

void RequestTimeSkip::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Hours = Serialization::ReadFloat(aReader);
}
