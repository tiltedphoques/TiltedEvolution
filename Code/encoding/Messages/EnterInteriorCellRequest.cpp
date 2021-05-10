#include <Messages/EnterInteriorCellRequest.h>
#include <TiltedCore/Serialization.hpp>

void EnterInteriorCellRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    CellId.Serialize(aWriter);
}

void EnterInteriorCellRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    CellId.Deserialize(aReader);
}
