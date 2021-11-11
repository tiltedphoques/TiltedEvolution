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
               ContactNormalX == acRhs.ContactNormalX &&
               ContactNormalY == acRhs.ContactNormalY &&
               ContactNormalZ == acRhs.ContactNormalZ &&
               ProjectileBaseID == acRhs.ProjectileBaseID &&
               WeaponID == acRhs.WeaponID &&
               AmmoID == acRhs.AmmoID &&
               ZAngle == acRhs.ZAngle &&
               XAngle == acRhs.XAngle &&
               YAngle == acRhs.YAngle &&
               ParentCellID == acRhs.ParentCellID &&
               SpellID == acRhs.SpellID &&
               CastingSource == acRhs.CastingSource &&
               unkBool1 == acRhs.unkBool1 &&
               Area == acRhs.Area &&
               Power == acRhs.Power &&
               Scale == acRhs.Scale &&
               AlwaysHit == acRhs.AlwaysHit &&
               NoDamageOutsideCombat == acRhs.NoDamageOutsideCombat &&
               AutoAim == acRhs.AutoAim &&
               UseOrigin == acRhs.UseOrigin &&
               DeferInitialization == acRhs.DeferInitialization &&
               Tracer == acRhs.Tracer &&
               ForceConeOfFire == acRhs.ForceConeOfFire &&
               GetOpcode() == acRhs.GetOpcode();
    }

    uint32_t ShooterID{};
    float OriginX{};
    float OriginY{};
    float OriginZ{};
    float ContactNormalX{};
    float ContactNormalY{};
    float ContactNormalZ{};
    GameId ProjectileBaseID{};
    GameId WeaponID{};
    GameId AmmoID{};
    float ZAngle{};
    float XAngle{};
    float YAngle{};
    GameId ParentCellID{};
    GameId SpellID{};
    int32_t CastingSource{};
    bool unkBool1{};
    int32_t Area{};
    float Power{};
    float Scale{};
    bool AlwaysHit{};
    bool NoDamageOutsideCombat{};
    bool AutoAim{};
    bool UseOrigin{};
    bool DeferInitialization{};
    bool Tracer{};
    bool ForceConeOfFire{};
};
