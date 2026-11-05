#include <Structs/Inventory.h>
#include <Decoding.h>
#include <TiltedCore/Serialization.hpp>
#include <algorithm>
#include <cmath>

using TiltedPhoques::Serialization;

bool Inventory::PotionData::operator==(const PotionData& acRhs) const noexcept
{
    return IsPoison == acRhs.IsPoison && Effects.size() == acRhs.Effects.size() &&
        std::is_permutation(Effects.begin(), Effects.end(), acRhs.Effects.begin());
}

bool Inventory::PotionData::IsValid() const noexcept
{
    if (Effects.empty() || Effects.size() > kMaxEffects)
        return false;
    for (const auto& effect : Effects)
        if (!std::isfinite(effect.Magnitude) || effect.Magnitude < 0 || !std::isfinite(effect.RawCost) || effect.RawCost < 0 ||
            effect.Area < 0 || effect.Duration < 0 || effect.EffectId.ModId == 0xFFFFFFFF || !effect.EffectId.BaseId)
            return false;
    return true;
}

void Inventory::PotionData::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    // Most entries are not crafted potions, so a single bit stands in for an empty recipe.
    const bool cPresent = IsPoison || !Effects.empty();
    Serialization::WriteBool(aWriter, cPresent);
    if (!cPresent)
        return;

    Serialization::WriteBool(aWriter, IsPoison);
    Serialization::WriteVarInt(aWriter, Effects.size());
    for (const auto& effect : Effects)
        effect.Serialize(aWriter);
}

void Inventory::PotionData::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    IsPoison = false;
    Effects.clear();
    if (!Serialization::ReadBool(aReader))
        return;

    IsPoison = Serialization::ReadBool(aReader);
    Decoding::ReadList(aReader, Effects, kMaxEffects);
}

void Inventory::EffectItem::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, Magnitude);
    Serialization::WriteVarInt(aWriter, Area);
    Serialization::WriteVarInt(aWriter, Duration);
    Serialization::WriteFloat(aWriter, RawCost);
    EffectId.Serialize(aWriter);
}

void Inventory::EffectItem::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Magnitude = Serialization::ReadFloat(aReader);
    Area = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Duration = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    RawCost = Serialization::ReadFloat(aReader);
    EffectId.Deserialize(aReader);
}

void Inventory::Entry::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    BaseId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, Count);

    Serialization::WriteFloat(aWriter, ExtraCharge);

    ExtraEnchantId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, ExtraEnchantCharge);
    Serialization::WriteVarInt(aWriter, EnchantData.Effects.size());
    for (const EffectItem& effect : EnchantData.Effects)
    {
        effect.Serialize(aWriter);
    }

    Serialization::WriteFloat(aWriter, ExtraHealth);

    ExtraPoisonId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, ExtraPoisonCount);

    Serialization::WriteVarInt(aWriter, ExtraSoulLevel);

    Serialization::WriteBool(aWriter, EnchantData.IsWeapon);
    Serialization::WriteBool(aWriter, ExtraEnchantRemoveUnequip);
    Serialization::WriteBool(aWriter, ExtraWorn);
    Serialization::WriteBool(aWriter, ExtraWornLeft);
    Serialization::WriteBool(aWriter, IsQuestItem);
    Potion.Serialize(aWriter);
    PoisonData.Serialize(aWriter);
}

void Inventory::Entry::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    BaseId.Deserialize(aReader);
    Count = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    ExtraCharge = Serialization::ReadFloat(aReader);

    ExtraEnchantId.Deserialize(aReader);
    ExtraEnchantCharge = Serialization::ReadVarInt(aReader) & 0xFFFF;
    Decoding::ReadList(aReader, EnchantData.Effects, kMaxEffects);

    ExtraHealth = Serialization::ReadFloat(aReader);

    ExtraPoisonId.Deserialize(aReader);
    ExtraPoisonCount = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    ExtraSoulLevel = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    EnchantData.IsWeapon = Serialization::ReadBool(aReader);
    ExtraEnchantRemoveUnequip = Serialization::ReadBool(aReader);
    ExtraWorn = Serialization::ReadBool(aReader);
    ExtraWornLeft = Serialization::ReadBool(aReader);
    IsQuestItem = Serialization::ReadBool(aReader);
    Potion.Deserialize(aReader);
    PoisonData.Deserialize(aReader);
}

bool Inventory::operator==(const Inventory& acRhs) const noexcept
{
    return Entries == acRhs.Entries;
}

bool Inventory::operator!=(const Inventory& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

bool Inventory::Entry::operator==(const Inventory::Entry& acRhs) const noexcept
{
    return SameBase(acRhs) && Count == acRhs.Count && IsExtraDataEquals(acRhs);
}

bool Inventory::Entry::operator!=(const Inventory::Entry& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Inventory::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Entries.size());
    for (const Entry& entry : Entries)
    {
        entry.Serialize(aWriter);
    }

    CurrentMagicEquipment.Serialize(aWriter);
}

void Inventory::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Decoding::ReadList(aReader, Entries, kMaxEntries);

    CurrentMagicEquipment.Deserialize(aReader);
}

std::optional<Inventory::Entry> Inventory::GetEntryById(GameId& aItemId) const noexcept
{
    auto entry = std::find_if(Entries.begin(), Entries.end(), [aItemId](auto entry) { return entry.BaseId == aItemId; });
    if (entry == Entries.end())
        return std::nullopt;

    return {*entry};
}

int32_t Inventory::GetEntryCountById(GameId& aItemId) const noexcept
{
    auto entry = GetEntryById(aItemId);
    if (!entry)
        return 0;

    return entry->Count;
}

// TODO: unit testing
void Inventory::AddOrRemoveEntry(const Entry& acEntry) noexcept
{
    auto duplicate = std::find_if(Entries.begin(), Entries.end(), [acEntry](Entry& entry) { return entry.CanBeMerged(acEntry); });

    if (duplicate != Entries.end())
    {
        duplicate->Count += acEntry.Count;
        if (duplicate->Count <= 0)
            Entries.erase(duplicate);
    }
    else
    {
        Entries.push_back(acEntry);
    }
}

void Inventory::UpdateEquipment(const Inventory& acNewInventory) noexcept
{
    while (true)
    {
        auto wornEntry = std::find_if(Entries.begin(), Entries.end(), [](auto& aEntry) { return aEntry.IsWorn(); });
        if (wornEntry == Entries.end())
            break;

        wornEntry->ExtraWorn = wornEntry->ExtraWornLeft = false;
    }

    for (const auto& newEntry : acNewInventory.Entries)
    {
        if (!newEntry.IsWorn())
            continue;

        auto entry = std::find_if(Entries.begin(), Entries.end(), [&newEntry](auto& aEntry) { return aEntry.BaseId == newEntry.BaseId; });

        // This shouldn't happen
        if (entry == Entries.end())
            continue;

        entry->ExtraWorn = newEntry.ExtraWorn;
        entry->ExtraWornLeft = newEntry.ExtraWornLeft;
    }

    CurrentMagicEquipment = acNewInventory.CurrentMagicEquipment;
}

void Inventory::RemoveByFilter(std::function<bool(const Entry&)> aFilter) noexcept
{
    Entries.erase(std::remove_if(Entries.begin(), Entries.end(), aFilter), Entries.end());
}

bool Inventory::ContainsQuestItems() const noexcept
{
    for (const auto& entry : Entries)
    {
        if (entry.IsQuestItem)
            return true;
    }
    return false;
}
