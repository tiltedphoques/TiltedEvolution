#include <Messages/AssignObjectResponse.h>

void AssignObjectResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    CurrentLockData.Serialize(aWriter);
}

void AssignObjectResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    CurrentLockData.Deserialize(aReader);
}
