#pragma once

#include "Message.h"
#include <Structs/Mods.h>
#include <Structs/ActorValues.h>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/GameId.h>

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
               Owner == achRhs.Owner &&
               Cookie == achRhs.Cookie &&
               ServerId == achRhs.ServerId &&
               Position == achRhs.Position &&
               CellId == achRhs.CellId &&
               WorldSpaceId == achRhs.WorldSpaceId &&
               AllActorValues == achRhs.AllActorValues &&
               IsDead == achRhs.IsDead &&
               IsWeaponDrawn == achRhs.IsWeaponDrawn;
    }

    bool Owner{ false };
    uint32_t Cookie{};
    uint32_t ServerId{};
    Vector3_NetQuantize Position{};
    GameId CellId{};
    GameId WorldSpaceId{};
    ActorValues AllActorValues{};
    bool IsDead{};
    bool IsWeaponDrawn{};
};
