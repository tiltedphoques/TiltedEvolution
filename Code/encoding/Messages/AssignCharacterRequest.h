#pragma once

#include "Message.h"
#include <Structs/GameId.h>
#include <Structs/ActionEvent.h>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/Rotator2_NetQuantize.h>
#include <Structs/Tints.h>
#include <Structs/Inventory.h>
#include <Structs/Factions.h>
#include <Structs/QuestLog.h>
#include <Structs/ActorValues.h>

using TiltedPhoques::String;

struct AssignCharacterRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kAssignCharacterRequest;

    AssignCharacterRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~AssignCharacterRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const AssignCharacterRequest& acRhs) const noexcept
    {
        return Cookie == acRhs.Cookie &&
            ReferenceId == acRhs.ReferenceId &&
            FormId == acRhs.FormId &&
            CellId == acRhs.CellId &&
            Position == acRhs.Position &&
            Rotation == acRhs.Rotation &&
            ChangeFlags == acRhs.ChangeFlags &&
            AppearanceBuffer == acRhs.AppearanceBuffer &&
            InventoryContent == acRhs.InventoryContent &&
            FactionsContent == acRhs.FactionsContent &&
            LatestAction == acRhs.LatestAction &&
            FaceTints == acRhs.FaceTints &&
            QuestContent == acRhs.QuestContent &&
            AllActorValues == acRhs.AllActorValues &&
            GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t Cookie{};
    GameId ReferenceId{};
    GameId FormId{};
    GameId CellId{};
    Vector3_NetQuantize Position{};
    Rotator2_NetQuantize Rotation{};
    uint32_t ChangeFlags{};
    String AppearanceBuffer{};
    Inventory InventoryContent{};
    Factions FactionsContent{};
    ActionEvent LatestAction{};
    QuestLog QuestContent{};
    Tints FaceTints{};
    ActorValues AllActorValues{};
};
