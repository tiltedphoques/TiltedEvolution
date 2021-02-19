#include <Structs/Mods.h>
#include <TiltedCore/Serialization.hpp>
#include <algorithm>

using TiltedPhoques::Serialization;

bool Mods::operator==(const Mods& acRhs) const noexcept
{
    return StandardMods == acRhs.StandardMods && LiteMods == acRhs.LiteMods;
}

bool Mods::operator!=(const Mods& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Mods::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    const uint8_t standardCount = StandardMods.size() & 0xFF;
    aWriter.WriteBits(standardCount, 8); // 255 max
    for (auto& entry : StandardMods)
    {
        aWriter.WriteBits(entry.Id, 8); // standard mods can not exceed 254
        Serialization::WriteString(aWriter, entry.Filename);
    }

    // Lite mods can not exceed 4096
    const uint16_t liteCount = std::min(LiteMods.size(), size_t(4096));
    aWriter.WriteBits(liteCount, 13); 
    for (auto& entry : LiteMods)
    {
        aWriter.WriteBits(entry.Id, 12); // Lite id can not exceed 4095
        Serialization::WriteString(aWriter, entry.Filename);
    }
}

void Mods::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t data = 0;
    aReader.ReadBits(data, 8);

    const size_t standardCount = data & 0xFF;
    StandardMods.resize(standardCount);
    for (size_t i = 0; i < standardCount; ++i)
    {
        aReader.ReadBits(data, 8);
        StandardMods[i].Id = data & 0xFF;
        StandardMods[i].Filename = Serialization::ReadString(aReader);
    }

    aReader.ReadBits(data, 13);
    const size_t liteCount = data & 0xFFFF;
    LiteMods.resize(liteCount);
    for (size_t i = 0; i < liteCount; ++i)
    {
        aReader.ReadBits(data, 12);
        LiteMods[i].Id = data & 0xFFF;
        LiteMods[i].Filename = Serialization::ReadString(aReader);
    }
}
