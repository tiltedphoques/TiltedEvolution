#pragma once

#include <Structs/GameId.h>

struct SettlementObject
{
    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    bool operator==(const SettlementObject& acOther) const noexcept;
    bool IsValid() const noexcept;

    uint32_t Id{};
    uint32_t Revision{};
    uint32_t LeaderReference{};
    GameId Workshop{};
    GameId Reference{};
    GameId Base{};
    GameId Cell{};
    GameId WorldSpace{};
    glm::vec3 Position{};
    glm::vec3 Rotation{};
    uint16_t Scale{100};
    bool Removed{};
};
