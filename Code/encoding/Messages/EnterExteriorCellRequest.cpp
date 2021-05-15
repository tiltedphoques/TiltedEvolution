#include <Messages/EnterExteriorCellRequest.h>
#include <TiltedCore/Serialization.hpp>

void EnterExteriorCellRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    WorldSpaceId.Serialize(aWriter);
    CellId.Serialize(aWriter);
    CurrentCoords.Serialize(aWriter);
}

void EnterExteriorCellRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    WorldSpaceId.Deserialize(aReader);
    CellId.Deserialize(aReader);
    CurrentCoords.Deserialize(aReader);
}
