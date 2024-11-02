#pragma once

/**
 * @brief Dispatched when a local projectile has been launched.
 */
struct ProjectileLaunchedEvent
{
    NiPoint3 Origin;
    uint32_t ProjectileBaseID;
    uint32_t ShooterID;
    uint32_t WeaponID;
    uint32_t AmmoID;
    float ZAngle;
    float XAngle;
    float YAngle;
    uint32_t ParentCellID;
    uint32_t SpellID;
    int32_t CastingSource;
    int32_t Area;
    float Power;
    float Scale;
    bool AlwaysHit;
    bool NoDamageOutsideCombat;
    bool AutoAim;
    bool DeferInitialization;
    bool ForceConeOfFire;
    bool UnkBool1;
    bool UnkBool2;
};
