#include <Structs/PowerArmorData.h>
#include <cmath>

void PowerArmorData::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Items.Serialize(aWriter);
    TiltedPhoques::Serialization::WriteFloat(aWriter, BatteryCharge);
    TiltedPhoques::Serialization::WriteVarInt(aWriter, FrameToken);
}

void PowerArmorData::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Items = {};
    Items.Deserialize(aReader);
    BatteryCharge = TiltedPhoques::Serialization::ReadFloat(aReader);
    FrameToken = TiltedPhoques::Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}

bool PowerArmorData::operator==(const PowerArmorData& acOther) const noexcept
{
    if (BatteryCharge != acOther.BatteryCharge || FrameToken != acOther.FrameToken || Items != acOther.Items)
        return false;
    for (size_t i = 0; i < Items.Entries.size(); ++i)
    {
        if (Items.Entries[i].Mods != acOther.Items.Entries[i].Mods)
            return false;
    }
    return true;
}

bool PowerArmorData::IsValid() const noexcept
{
    if (!std::isfinite(BatteryCharge) || BatteryCharge < 0.f || BatteryCharge > 100000.f || Items.Entries.size() > 64)
        return false;
    for (const auto& item : Items.Entries)
    {
        if (item.BaseId == GameId{} || item.Count <= 0 || item.Count > 10000 || item.Mods.size() > 32 ||
            !std::isfinite(item.ExtraHealth) || item.ExtraHealth < 0.f || item.ExtraHealth > 1.f)
            return false;
    }
    return true;
}
