#include "ServerSettings.h"
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool ServerSettings::operator==(const ServerSettings& acRhs) const noexcept
{
    return Difficulty == acRhs.Difficulty &&
           GreetingsEnabled == acRhs.GreetingsEnabled &&
           PvpEnabled == acRhs.PvpEnabled &&
           SyncPlayerHomes == acRhs.SyncPlayerHomes &&
           DeathSystemEnabled == acRhs.DeathSystemEnabled;
}

bool ServerSettings::operator!=(const ServerSettings& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void ServerSettings::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Difficulty);
    Serialization::WriteBool(aWriter, GreetingsEnabled);
    Serialization::WriteBool(aWriter, PvpEnabled);
    Serialization::WriteBool(aWriter, SyncPlayerHomes);
    Serialization::WriteBool(aWriter, DeathSystemEnabled);
}

void ServerSettings::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Difficulty = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    GreetingsEnabled = Serialization::ReadBool(aReader);
    PvpEnabled = Serialization::ReadBool(aReader);
    SyncPlayerHomes = Serialization::ReadBool(aReader);
    DeathSystemEnabled = Serialization::ReadBool(aReader);
}

