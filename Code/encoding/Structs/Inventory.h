#pragma once

#include <TiltedCore/Stl.hpp>
#include <Structs/GameId.h>

using TiltedPhoques::Buffer;
using TiltedPhoques::String;

struct Inventory
{
    Inventory() = default;
    ~Inventory() = default;

    bool operator==(const Inventory& acRhs) const noexcept;
    bool operator!=(const Inventory& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    String Buffer{};
    bool IsWeaponDrawn{};
    GameId RightHandWeapon{};

#if TP_SKYRIM
    GameId LeftHandWeapon{};
    GameId LeftHandSpell{};
    GameId RightHandSpell{};
    GameId Shout{};
    GameId Ammo{};
#endif
};
