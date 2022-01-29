#include <Structs/Container.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

void Container::Entry::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    BaseId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, Count);
    Serialization::WriteFloat(aWriter, ExtraCharge);
    ExtraEnchantId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, ExtraEnchantCharge);
    Serialization::WriteFloat(aWriter, ExtraHealth);
    ExtraPoisonId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, ExtraPoisonCount);
    Serialization::WriteVarInt(aWriter, ExtraSoulLevel);
    Serialization::WriteString(aWriter, ExtraTextDisplayName);

    Serialization::WriteBool(aWriter, ExtraEnchantRemoveUnequip);
    Serialization::WriteBool(aWriter, ExtraWorn);
    Serialization::WriteBool(aWriter, ExtraWornLeft);
}

void Container::Entry::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    BaseId.Deserialize(aReader);
    Count = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ExtraCharge = Serialization::ReadFloat(aReader);
    ExtraEnchantId.Deserialize(aReader);
    ExtraEnchantCharge = Serialization::ReadVarInt(aReader) & 0xFFFF;
    ExtraHealth = Serialization::ReadFloat(aReader);
    ExtraPoisonId.Deserialize(aReader);
    ExtraPoisonCount = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ExtraSoulLevel = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    ExtraTextDisplayName = Serialization::ReadString(aReader);

    ExtraEnchantRemoveUnequip = Serialization::ReadBool(aReader);
    ExtraWorn = Serialization::ReadBool(aReader);
    ExtraWornLeft = Serialization::ReadBool(aReader);
}

bool Container::operator==(const Container& acRhs) const noexcept
{
    return Entries == acRhs.Entries;
}

bool Container::operator!=(const Container& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

bool Container::Entry::operator==(const Container::Entry& acRhs) const noexcept
{
    return BaseId == acRhs.BaseId &&
           Count == acRhs.Count &&
           ExtraTextDisplayName == acRhs.ExtraTextDisplayName &&
           IsExtraDataEquals(acRhs);
}

bool Container::Entry::operator!=(const Container::Entry& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Container::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Entries.size());
    for (const Entry& entry : Entries)
    {
        entry.Serialize(aWriter);
    }
}

void Container::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint32_t count = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    for (uint32_t i = 0; i < count; i++)
    {
        Entry entry;
        entry.Deserialize(aReader);
        Entries.push_back(entry);
    }
}
