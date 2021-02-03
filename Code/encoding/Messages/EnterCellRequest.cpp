#include <Messages/EnterCellRequest.h>
#include <TiltedCore/Serialization.hpp>

void EnterCellRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    CellId.Serialize(aWriter);
}

void EnterCellRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    CellId.Deserialize(aReader);
}
