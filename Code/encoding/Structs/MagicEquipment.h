#pragma once

#include <Structs/GameId.h>

struct RequestEquipmentChanges;

struct MagicEquipment
{
    bool operator==(const MagicEquipment& acRhs) const noexcept;
    bool operator!=(const MagicEquipment& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    GameId LeftHandSpell{};
    GameId RightHandSpell{};
    GameId Shout{};
};
