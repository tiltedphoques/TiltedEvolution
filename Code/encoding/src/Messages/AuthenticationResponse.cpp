#include <Messages/AuthenticationResponse.h>

void AuthenticationResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Accepted);
    Mods.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, Scripts.size());
    aWriter.WriteBytes(Scripts.data(), Scripts.size());
    Serialization::WriteVarInt(aWriter, ReplicatedObjects.size());
    aWriter.WriteBytes(ReplicatedObjects.data(), ReplicatedObjects.size());
}

void AuthenticationResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Accepted = Serialization::ReadBool(aReader);
    Mods.Deserialize(aReader);
    const auto scriptsLength = Serialization::ReadVarInt(aReader);
    Scripts.resize(scriptsLength);
    aReader.ReadBytes(Scripts.data(), scriptsLength);
    const auto replicatedObjectsLength = Serialization::ReadVarInt(aReader);
    ReplicatedObjects.resize(replicatedObjectsLength);
    aReader.ReadBytes(ReplicatedObjects.data(), replicatedObjectsLength);
}
