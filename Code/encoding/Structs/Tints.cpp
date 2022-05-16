#include <Structs/Tints.h>
#include <TiltedCore/Serialization.hpp>
#include <stdexcept>

using TiltedPhoques::Serialization;

bool Tints::Entry::operator==(const Entry& acRhs) const noexcept
{
    return Alpha == acRhs.Alpha &&
        Type == acRhs.Type &&
        Color == acRhs.Color &&
        Name == acRhs.Name;
}

bool Tints::Entry::operator!=(const Entry& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

bool Tints::operator==(const Tints& acRhs) const noexcept
{
    return Entries == acRhs.Entries;
}

bool Tints::operator!=(const Tints& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Tints::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Entries.size(), 8);

    for (auto& entry : Entries)
    {
        Serialization::WriteVarInt(aWriter, entry.Type);
        aWriter.WriteBits(entry.Color, 32);
        entry.Name.Serialize(aWriter);
        aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&entry.Alpha), 32);
    }
}

void Tints::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t tmp = 0;
    aReader.ReadBits(tmp, 8);

    const auto cCount = tmp & 0xFF;

    for (auto i = 0u; i < cCount; ++i)
    {
        uint64_t buffer = 0;

        Entry entry;
        entry.Type = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

        aReader.ReadBits(buffer, 32);
        entry.Color = buffer & 0xFFFFFFFF;
        entry.Name.Deserialize(aReader);

        aReader.ReadBits(buffer, 32);
        tmp = buffer & 0xFFFFFFFF;
        entry.Alpha = *reinterpret_cast<float*>(&tmp);

        Entries.push_back(entry);
    }
}
