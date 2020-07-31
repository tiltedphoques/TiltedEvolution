#include <Structs/Inventory.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool Inventory::operator==(const Inventory& acRhs) const noexcept
{
    return RightHandWeapon == acRhs.RightHandWeapon
        && Buffer == acRhs.Buffer
#if TP_SKYRIM
        && LeftHandWeapon == acRhs.LeftHandWeapon
        && RightHandSpell == acRhs.RightHandSpell
        && LeftHandSpell == acRhs.LeftHandSpell
        && Shout == acRhs.Shout
#endif
        ;
}

bool Inventory::operator!=(const Inventory& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Inventory::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    bool isRightWeaponSet = RightHandWeapon != GameId{};
#if TP_SKYRIM
    bool isLeftWeaponSet = LeftHandWeapon != GameId{};
    bool isLeftSpellSet = LeftHandSpell != GameId{};
    bool isRightSpellSet = RightHandSpell != GameId{};
    bool isShoutSet = Shout != GameId{};
#endif

    Serialization::WriteString(aWriter, Buffer);

    Serialization::WriteBool(aWriter, isRightWeaponSet);
#if TP_SKYRiM
    Serialization::WriteBool(aWriter, isLeftWeaponSet);
    Serialization::WriteBool(aWriter, isLeftSpellSet);
    Serialization::WriteBool(aWriter, isRightSpellSet);
    Serialization::WriteBool(aWriter, isShoutSet);
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
#endif
}

void Inventory::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Buffer = Serialization::ReadString(aReader);

    bool isRightWeaponSet = Serialization::ReadBool(aReader);
#if TP_SKYRIM
    bool isLeftWeaponSet = Serialization::ReadBool(aReader);
    bool isLeftSpellSet = Serialization::ReadBool(aReader);
    bool isRightSpellSet = Serialization::ReadBool(aReader);
    bool isShoutSet = Serialization::ReadBool(aReader);
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
#endif

}
