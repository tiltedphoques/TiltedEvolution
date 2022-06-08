#pragma once

#include "Message.h"
#include <Structs/Mods.h>
#include <Structs/ActorValues.h>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/GameId.h>
#include <Structs/Inventory.h>

struct AssignCharacterResponse final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kAssignCharacterResponse;

    AssignCharacterResponse() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AssignCharacterResponse& achRhs) const noexcept
    {
        return GetOpcode() == achRhs.GetOpcode() &&
               Cookie == achRhs.Cookie &&
               ServerId == achRhs.ServerId &&
               PlayerId == achRhs.PlayerId &&
               Position == achRhs.Position &&
               CellId == achRhs.CellId &&
               WorldSpaceId == achRhs.WorldSpaceId &&
               AllActorValues == achRhs.AllActorValues &&
               CurrentInventory == achRhs.CurrentInventory &&
               Owner == achRhs.Owner &&
               IsDead == achRhs.IsDead &&
               IsWeaponDrawn == achRhs.IsWeaponDrawn;
    }

    uint32_t Cookie{};
    uint32_t ServerId{};
    uint32_t PlayerId{};
    Vector3_NetQuantize Position{};
    GameId CellId{};
    GameId WorldSpaceId{};
    ActorValues AllActorValues{};
    Inventory CurrentInventory{};
    bool Owner{ false };
    bool IsDead{};
    bool IsWeaponDrawn{};
};
