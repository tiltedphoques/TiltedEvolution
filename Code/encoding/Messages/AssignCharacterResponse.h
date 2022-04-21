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
               AllActorValues == achRhs.AllActorValues &&
               ChangeFlags == achRhs.ChangeFlags &&
               AppearanceBuffer == achRhs.AppearanceBuffer &&
               FaceTints == achRhs.FaceTints &&
               IsDead == achRhs.IsDead &&
               IsWeaponDrawn == achRhs.IsWeaponDrawn &&
               IsLeveledActor == achRhs.IsLeveledActor;
    }

    bool Owner{ false };
    uint32_t Cookie{};
    uint32_t ServerId{};
    Vector3_NetQuantize Position{};
    GameId CellId{};
    ActorValues AllActorValues{};
    uint32_t ChangeFlags{};
    String AppearanceBuffer{};
    Tints FaceTints{};
    bool IsDead{};
    bool IsWeaponDrawn{};
    bool IsLeveledActor{};
};
