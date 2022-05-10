#include "ServerSettings.h"
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool ServerSettings::operator==(const ServerSettings& acRhs) const noexcept
{
    return Difficulty == acRhs.Difficulty;
}

bool ServerSettings::operator!=(const ServerSettings& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void ServerSettings::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Difficulty);
    Serialization::WriteBool(aWriter, EnableGreetings);
}

void ServerSettings::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Difficulty = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    EnableGreetings = Serialization::ReadBool(aReader);
}

