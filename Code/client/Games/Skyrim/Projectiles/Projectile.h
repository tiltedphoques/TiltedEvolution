#pragma once

#include <Games/Skyrim/TESObjectREFR.h>
#include <Games/Magic/MagicSystem.h>

struct BGSProjectile;
struct TESObjectREFR;
struct TESObjectWEAP;
struct TESAmmo;
struct TESObjectCELL;
struct MagicItem;
struct AlchemyItem;
struct CombatController;
struct TESForm;

struct Projectile : TESObjectREFR
{
    struct LaunchData
    {
        virtual ~LaunchData() = default;

        // When a player is shooting an arrow, all remaining uknown fields are null
        NiPoint3 Origin;
        NiPoint3 ContactNormal; // always null init?
        TESForm* pProjectileBase; // is actually BGSProjectile*
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
        bool unkBool1;
        uint8_t unk7D[0xB]; // first byte is set when Faendal shoots a bow. May or may not be significant.
        AlchemyItem* pPoison;
        int32_t iArea; // unsure // set to 0 in TESObjectWEAP::Fire()
        float fPower; // unsure // init to 1.0 and set to 1.0 before launch in TESObjectWEAP::Fire()
        float fScale; // unsure // init to 1.0 and set to 1.0 before launch in TESObjectWEAP::Fire()
        bool bAlwaysHit; // init to true
        bool bNoDamageOutsideCombat; // init to false
        // TODO: The following bools are probably something else
        bool bAutoAim; // unsure // init to false
        bool bUseOrigin; // unsure // init to false
        bool bDeferInitialization; // unsure // init to false
        bool bTracer; // unsure // init to false
        bool bForceConeOfFire; // unsure // init to false
    };

    static void* Launch(void* apResult, LaunchData& arData) noexcept;
};

static_assert(sizeof(Projectile::LaunchData) == 0xA8);

