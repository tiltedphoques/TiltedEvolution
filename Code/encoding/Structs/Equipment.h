#pragma once

#include <Structs/GameId.h>

struct RequestMagicEquipmentChanges;

struct MagicEquipment
{
    bool operator==(const MagicEquipment& acRhs) const noexcept;
    bool operator!=(const MagicEquipment& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    void UpdateEquipment(const RequestMagicEquipmentChanges& acChanges) noexcept;

    GameId LeftHandSpell{};
    GameId RightHandSpell{};
    GameId Shout{};
};
