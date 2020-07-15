#include <Messages/AuthenticationResponse.h>

void AuthenticationResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, Accepted);
    Mods.Serialize(aWriter);
    Scripts.Serialize(aWriter);
    ReplicatedObjects.Serialize(aWriter);
}

void AuthenticationResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Accepted = Serialization::ReadBool(aReader);
    Mods.Deserialize(aReader);
    Scripts.Deserialize(aReader);
    ReplicatedObjects.Deserialize(aReader);
}
