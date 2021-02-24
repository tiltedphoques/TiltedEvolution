#include <Structs/Faction.h>

bool Faction::operator==(const Faction& acRhs) const noexcept
{
    return Id == acRhs.Id &&
        Rank == acRhs.Rank;
}

bool Faction::operator!=(const Faction& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Faction::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    aWriter.WriteBits(Rank, 8);
}

void Faction::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Id.Deserialize(aReader);

    uint64_t tmp;
    aReader.ReadBits(tmp, 8);
    Rank = tmp & 0xFF;
}
