#include <Structs/Equipment.h>
#include <TiltedCore/Serialization.hpp>
#include <Messages/RequestEquipmentChanges.h>

using TiltedPhoques::Serialization;

bool Equipment::operator==(const Equipment& acRhs) const noexcept
{
    return RightHandWeapon == acRhs.RightHandWeapon
#if TP_SKYRIM
           && LeftHandWeapon == acRhs.LeftHandWeapon
           && LeftHandSpell == acRhs.LeftHandSpell
           && RightHandSpell == acRhs.RightHandSpell
           && Shout == acRhs.Shout
           && Ammo == acRhs.Ammo
#endif
           ;
}

bool Equipment::operator!=(const Equipment& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Equipment::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    bool isRightWeaponSet = RightHandWeapon != GameId{};
#if TP_SKYRIM
    bool isLeftWeaponSet = LeftHandWeapon != GameId{};
    bool isLeftSpellSet = LeftHandSpell != GameId{};
    bool isRightSpellSet = RightHandSpell != GameId{};
    bool isShoutSet = Shout != GameId{};
    bool isAmmoSet = Ammo != GameId{};
#endif

    Serialization::WriteBool(aWriter, isRightWeaponSet);
#if TP_SKYRIM
    Serialization::WriteBool(aWriter, isLeftWeaponSet);
    Serialization::WriteBool(aWriter, isLeftSpellSet);
    Serialization::WriteBool(aWriter, isRightSpellSet);
    Serialization::WriteBool(aWriter, isShoutSet);
    Serialization::WriteBool(aWriter, isAmmoSet);
#endif

    if (isRightWeaponSet)
        RightHandWeapon.Serialize(aWriter);

#if TP_SKYRIM
    if (isLeftWeaponSet)
        LeftHandWeapon.Serialize(aWriter);

    if (isLeftSpellSet)
        LeftHandSpell.Serialize(aWriter);

    if (isRightSpellSet)
        RightHandSpell.Serialize(aWriter);

    if (isShoutSet)
        Shout.Serialize(aWriter);

    if (isAmmoSet)
        Ammo.Serialize(aWriter);
#endif
}

void Equipment::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    bool isRightWeaponSet = Serialization::ReadBool(aReader);
#if TP_SKYRIM
    bool isLeftWeaponSet = Serialization::ReadBool(aReader);
    bool isLeftSpellSet = Serialization::ReadBool(aReader);
    bool isRightSpellSet = Serialization::ReadBool(aReader);
    bool isShoutSet = Serialization::ReadBool(aReader);
    bool isAmmoSet = Serialization::ReadBool(aReader);
#endif

    if (isRightWeaponSet)
        RightHandWeapon.Deserialize(aReader);

#if TP_SKYRIM
    if (isLeftWeaponSet)
        LeftHandWeapon.Deserialize(aReader);

    if (isLeftSpellSet)
        LeftHandSpell.Deserialize(aReader);

    if (isRightSpellSet)
        RightHandSpell.Deserialize(aReader);

    if (isShoutSet)
        Shout.Deserialize(aReader);

    if (isAmmoSet)
        Ammo.Deserialize(aReader);
#endif
}

void Equipment::UpdateEquipment(const RequestEquipmentChanges& acChanges) noexcept
{
    if (acChanges.EquipSlotId.BaseId == 0x13F42) // if right-hand
    {
        if (acChanges.IsSpell)
            RightHandSpell = acChanges.Unequip ? GameId{} : acChanges.ItemId;
        else
            RightHandWeapon = acChanges.Unequip ? GameId{} : acChanges.ItemId;
    }
    else if (acChanges.EquipSlotId.BaseId == 0x13F43) // else if left-hand
    {
        if (acChanges.IsSpell)
            LeftHandSpell = acChanges.Unequip ? GameId{} : acChanges.ItemId;
        else
            LeftHandWeapon = acChanges.Unequip ? GameId{} : acChanges.ItemId;
    }
    else if (acChanges.IsShout)
    {
        Shout = acChanges.Unequip ? GameId{} : acChanges.ItemId;
    }
    else if (acChanges.IsAmmo)
    {
        Ammo = acChanges.Unequip ? GameId{} : acChanges.ItemId;
    }
}
