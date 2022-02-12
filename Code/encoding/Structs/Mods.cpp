#include <Structs/Mods.h>
#include <TiltedCore/Serialization.hpp>
#include <algorithm>

using TiltedPhoques::Serialization;

bool Mods::operator==(const Mods& acRhs) const noexcept
{
    return LoadOrder == acRhs.LoadOrder;
}

bool Mods::operator!=(const Mods& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Mods::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    const uint16_t modCount = std::min(LoadOrder.size(), size_t(4096)) & 0xFFFF;
    aWriter.WriteBits(modCount, 13); 

    for (auto& entry : LoadOrder)
    {
        aWriter.WriteBits(entry.Id, 12);
        Serialization::WriteString(aWriter, entry.Filename);
    }
}

void Mods::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t data = 0;
    aReader.ReadBits(data, 13);
    
    const size_t modCount = data & 0xFFFF;
    LoadOrder.resize(modCount);
    for (size_t i = 0; i < modCount; ++i)
    {
        aReader.ReadBits(data, 12);
        LoadOrder[i].Id = data & 0xFFF;
        LoadOrder[i].Filename = Serialization::ReadString(aReader);
    }
}
