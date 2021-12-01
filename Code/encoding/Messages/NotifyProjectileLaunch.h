#pragma once

#include "Message.h"
#include <Structs/GameId.h>

struct NotifyProjectileLaunch final : ServerMessage
{
    static constexpr ServerOpcode Opcode = kNotifyProjectileLaunch;

    NotifyProjectileLaunch() : ServerMessage(Opcode)
    {
    }

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    bool operator==(const NotifyProjectileLaunch& acRhs) const noexcept
    {
        return ShooterID == acRhs.ShooterID &&
               OriginX == acRhs.OriginX &&
               OriginY == acRhs.OriginY &&
               OriginZ == acRhs.OriginZ &&
               ProjectileBaseID == acRhs.ProjectileBaseID &&
               WeaponID == acRhs.WeaponID &&
               AmmoID == acRhs.AmmoID &&
               ZAngle == acRhs.ZAngle &&
               XAngle == acRhs.XAngle &&
               YAngle == acRhs.YAngle &&
               ParentCellID == acRhs.ParentCellID &&
               SpellID == acRhs.SpellID &&
               CastingSource == acRhs.CastingSource &&
               Area == acRhs.Area &&
               Power == acRhs.Power &&
               Scale == acRhs.Scale &&
               AlwaysHit == acRhs.AlwaysHit &&
               NoDamageOutsideCombat == acRhs.NoDamageOutsideCombat &&
               AutoAim == acRhs.AutoAim &&
               DeferInitialization == acRhs.DeferInitialization &&
               ForceConeOfFire == acRhs.ForceConeOfFire &&
               // Skyrim
               UnkBool1 == acRhs.UnkBool1 &&
               UnkBool2 == acRhs.UnkBool2 &&
               // Fallout 4
               ConeOfFireRadiusMult == acRhs.ConeOfFireRadiusMult &&
               Tracer == acRhs.Tracer &&
               IntentionalMiss == acRhs.IntentionalMiss &&
               Allow3D == acRhs.Allow3D &&
               Penetrates == acRhs.Penetrates &&
               IgnoreNearCollisions == acRhs.IgnoreNearCollisions &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t ShooterID{};
    float OriginX{};
    float OriginY{};
    float OriginZ{};
    GameId ProjectileBaseID{};
    GameId WeaponID{};
    GameId AmmoID{};
    float ZAngle{};
    float XAngle{};
    float YAngle{};
    GameId ParentCellID{};
    GameId SpellID{};
    int32_t CastingSource{};
    int32_t Area{};
    float Power{};
    float Scale{};
    bool AlwaysHit{};
    bool NoDamageOutsideCombat{};
    bool AutoAim{};
    bool DeferInitialization{};
    bool ForceConeOfFire{};

    // Skyrim
    bool UnkBool1{};
    bool UnkBool2{};

    // Fallout 4
    float ConeOfFireRadiusMult{};
    bool Tracer{};
    bool IntentionalMiss{};
    bool Allow3D{};
    bool Penetrates{};
    bool IgnoreNearCollisions{};
};
