#pragma once

#include <Games/Skyrim/TESObjectREFR.h>
#include <Games/Skyrim/Misc/MagicSystem.h>

struct BGSProjectile;
struct TESObjectREFR;
struct TESObjectWEAP;
struct TESAmmo;
struct TESObjectCELL;
struct MagicItem;
struct AlchemyItem;
struct CombatController;

struct Projectile : TESObjectREFR
{
    struct LaunchData
    {
        virtual ~LaunchData();

        NiPoint3 Origin;
        NiPoint3 ContactNormal; // always null init?
        BGSProjectile* pProjectileBase;
        TESObjectREFR* pShooter;
        CombatController* pCombatController;
        TESObjectWEAP* pFromWeapon;
        TESAmmo* pFromAmmo;
        float fZAngle;
        float fXAngle;
        float fYAngle;
        uint8_t unk54[0x14];
        TESObjectCELL* pParentCell;
        MagicItem* pSpell;
        MagicSystem::CastingSource eCastingSource;
        uint8_t unk7C[0xC];
        AlchemyItem* pPoison;
        int32_t iArea; // unsure // set to 0 in TESObjectWEAP::Fire()
        float fPower; // unsure // init to 1.0 and set to 1.0 before launch in TESObjectWEAP::Fire()
        float fScale; // unsure // init to 1.0 and set to 1.0 before launch in TESObjectWEAP::Fire()
        bool bAlwaysHit; // init to true
        bool bNoDamageOutsideCombat; // init to false
        bool bAutoAim; // unsure // init to false
        bool bUseOrigin; // unsure // init to false
        bool bDeferInitialization; // unsure // init to false
        bool bTracer; // unsure // init to false
        bool bForceConeOfFire; // unsure // init to false
    };

    void* Launch(void* apResult, LaunchData* arData) noexcept;
};

static_assert(sizeof(Projectile::LaunchData) == 0xA8);

