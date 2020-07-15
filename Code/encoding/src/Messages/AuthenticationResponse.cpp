#include <Messages/AuthenticationResponse.h>

void AuthenticationResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Accepted);
    Mods.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, Scripts.Data.size());
    aWriter.WriteBytes(Scripts.Data.data(), Scripts.Data.size());
    Serialization::WriteVarInt(aWriter, ReplicatedObjects.Data.size());
    aWriter.WriteBytes(ReplicatedObjects.Data.data(), ReplicatedObjects.Data.size());
}

void AuthenticationResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Accepted = Serialization::ReadBool(aReader);
    Mods.Deserialize(aReader);
    const auto scriptsLength = Serialization::ReadVarInt(aReader);
    Scripts.Data.resize(scriptsLength);
    aReader.ReadBytes(Scripts.Data.data(), scriptsLength);
    const auto replicatedObjectsLength = Serialization::ReadVarInt(aReader);
    ReplicatedObjects.Data.resize(replicatedObjectsLength);
    aReader.ReadBytes(ReplicatedObjects.Data.data(), replicatedObjectsLength);
}
