#include <Messages/AuthenticationResponse.h>

void AuthenticationResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Accepted);
    UserMods.Serialize(aWriter);
    ServerScripts.Serialize(aWriter);
    ReplicatedObjects.Serialize(aWriter);
}

void AuthenticationResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Accepted = Serialization::ReadBool(aReader);
    UserMods.Deserialize(aReader);
    ServerScripts.Deserialize(aReader);
    ReplicatedObjects.Deserialize(aReader);
}
