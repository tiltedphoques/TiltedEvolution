#pragma once

#include "Message.h"

#include <Structs/GameId.h>

struct ProjectileLaunchRequest final : ClientMessage
{
    static constexpr ClientOpcode Opcode = kProjectileLaunchRequest;

    ProjectileLaunchRequest() : ClientMessage(Opcode)
    {
    }

    virtual ~ProjectileLaunchRequest() = default;

    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;

    // TODO: operator==
    bool operator==(const ProjectileLaunchRequest& acRhs) const noexcept
    {
        return 
               GetOpcode() == acRhs.GetOpcode();
    }

    float OriginX{};
    float OriginY{};
    float OriginZ{};
    float ContactNormalX{};
    float ContactNormalY{};
    float ContactNormalZ{};
    // TODO: gameid
    uint32_t ProjectileBaseID{};
    uint32_t ShooterID{};
    uint32_t WeaponID{};
    uint32_t AmmoID{};
    float ZAngle{};
    float XAngle{};
    float YAngle{};
    uint32_t ParentCellID{};
    uint32_t SpellID{};
    uint32_t CastingSource{};
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
