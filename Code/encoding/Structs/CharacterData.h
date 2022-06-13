#pragma once

#include <TiltedCore/Buffer.hpp>
#include <Structs/GameId.h>
#include <Structs/GridCellCoords.h>
#include <Structs/ActionEvent.h>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/Rotator2_NetQuantize.h>
#include <Structs/Tints.h>
#include <Structs/Inventory.h>
#include <Structs/Factions.h>
#include <Structs/QuestLog.h>
#include <Structs/ActorValues.h>

using TiltedPhoques::String;

struct CharacterData
{
    CharacterData() = default;
    ~CharacterData() = default;

    bool operator==(const CharacterData& acRhs) const noexcept
    {
        return ReferenceId == acRhs.ReferenceId &&
            FormId == acRhs.FormId &&
            CellId == acRhs.CellId &&
            WorldSpaceId == acRhs.WorldSpaceId &&
            Position == acRhs.Position &&
            Rotation == acRhs.Rotation &&
            ChangeFlags == acRhs.ChangeFlags &&
            AppearanceBuffer == acRhs.AppearanceBuffer &&
            InventoryContent == acRhs.InventoryContent &&
            FactionsContent == acRhs.FactionsContent &&
            LatestAction == acRhs.LatestAction &&
            QuestContent == acRhs.QuestContent &&
            FaceTints == acRhs.FaceTints &&
            AllActorValues == acRhs.AllActorValues &&
            IsDead == acRhs.IsDead &&
            IsWeaponDrawn == acRhs.IsWeaponDrawn;
    }
    bool operator!=(const CharacterData& acRhs) const noexcept
    {
        return !this->operator==(acRhs);
    }

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    GameId ReferenceId{};
    GameId FormId{};
    GameId CellId{};
    GameId WorldSpaceId{};
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
    bool IsDead{};
    bool IsWeaponDrawn{};
};
