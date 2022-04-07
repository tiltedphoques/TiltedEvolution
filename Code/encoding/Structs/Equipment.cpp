#include <Structs/Equipment.h>
#include <TiltedCore/Serialization.hpp>
#include <Messages/RequestEquipmentChanges.h>

using TiltedPhoques::Serialization;

bool MagicEquipment::operator==(const MagicEquipment& acRhs) const noexcept
{
    return LeftHandSpell == acRhs.LeftHandSpell 
           && RightHandSpell == acRhs.RightHandSpell
           && Shout == acRhs.Shout;
}

bool MagicEquipment::operator!=(const MagicEquipment& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void MagicEquipment::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    bool isLeftSpellSet = LeftHandSpell != GameId{};
    bool isRightSpellSet = RightHandSpell != GameId{};
    bool isShoutSet = Shout != GameId{};

    Serialization::WriteBool(aWriter, isLeftSpellSet);
    Serialization::WriteBool(aWriter, isRightSpellSet);
    Serialization::WriteBool(aWriter, isShoutSet);

    if (isLeftSpellSet)
        LeftHandSpell.Serialize(aWriter);

    if (isRightSpellSet)
        RightHandSpell.Serialize(aWriter);

    if (isShoutSet)
        Shout.Serialize(aWriter);
}

void MagicEquipment::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    bool isLeftSpellSet = Serialization::ReadBool(aReader);
    bool isRightSpellSet = Serialization::ReadBool(aReader);
    bool isShoutSet = Serialization::ReadBool(aReader);

    if (isLeftSpellSet)
        LeftHandSpell.Deserialize(aReader);

    if (isRightSpellSet)
        RightHandSpell.Deserialize(aReader);

    if (isShoutSet)
        Shout.Deserialize(aReader);
}

void MagicEquipment::UpdateEquipment(const RequestMagicEquipmentChanges& acChanges) noexcept
{
    if (acChanges.IsSpell)
    {
        if (acChanges.EquipSlotId.BaseId == 0x13F42) // if right-hand
            RightHandSpell = acChanges.Unequip ? GameId{} : acChanges.ItemId;
        else if (acChanges.EquipSlotId.BaseId == 0x13F43) // else if left-hand
            LeftHandSpell = acChanges.Unequip ? GameId{} : acChanges.ItemId;
    }
    else if (acChanges.IsShout)
        Shout = acChanges.Unequip ? GameId{} : acChanges.ItemId;
}
