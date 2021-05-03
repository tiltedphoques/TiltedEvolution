#include <Messages/ShiftGridCellRequest.h>

void ShiftGridCellRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    WorldSpaceId.Serialize(aWriter);
    PlayerCell.Serialize(aWriter);
    CenterCoords.Serialize(aWriter);

    aWriter.WriteBits(Cells.size() & 0xFF, 8);

    for (const auto& cell : Cells)
    {
        cell.Serialize(aWriter);
    }
}

void ShiftGridCellRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    WorldSpaceId.Deserialize(aReader);
    PlayerCell.Deserialize(aReader);
    CenterCoords.Deserialize(aReader);

    uint64_t count = 0;
    aReader.ReadBits(count, 8);

    Cells.resize(count);

    for (auto i = 0u; i < count; ++i)
    {
        Cells[i].Deserialize(aReader);
    }
}
