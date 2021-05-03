#include <Structs/GridCellCoords.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

GridCellCoords::GridCellCoords(int32_t aX, int32_t aY) noexcept
    : X(aX)
    , Y(aY)
{
}

bool GridCellCoords::operator==(const GridCellCoords& acRhs) const noexcept
{
    return X == acRhs.X &&
        Y == acRhs.Y;
}

bool GridCellCoords::operator!=(const GridCellCoords& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void GridCellCoords::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, X);
    Serialization::WriteVarInt(aWriter, Y);
}

void GridCellCoords::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    X = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Y = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
