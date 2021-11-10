#pragma once

struct TESForm;
struct TESObjectREFR;

struct ProjectileLaunchedEvent
{
    NiPoint3 Origin;
    NiPoint3 ContactNormal;
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
    bool unkBool1;
    int32_t Area;
    float Power;
    float Scale;
    bool AlwaysHit;
    bool NoDamageOutsideCombat;
    bool AutoAim;
    bool UseOrigin;
    bool DeferInitialization;
    bool Tracer;
    bool ForceConeOfFire;

#if TP_FALLOUT4
    float ConeOfFireRadiusMult;
    bool IntentionalMiss;
    bool Allow3D;
    bool Penetrates;
    bool IgnoreNearCollisions;
#endif
};
