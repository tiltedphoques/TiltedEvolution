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
        NiPoint3 ContactNormal; // always null init? not syncing it now, idk if it matters
        TESForm* pProjectileBase; // is actually BGSProjectile*
        TESObjectREFR* pShooter;
        CombatController* pCombatController;
        TESObjectWEAP* pFromWeapon;
        TESAmmo* pFromAmmo;
        float fZAngle;
        float fXAngle;
        float fYAngle;
        uint8_t unk54[0x14]; // seemingly always null
        TESObjectCELL* pParentCell;
        MagicItem* pSpell;
        MagicSystem::CastingSource eCastingSource;
        bool bUnkBool1;
        uint8_t unk7D[0xB]; // seemingly always null
        AlchemyItem* pPoison;
        int32_t iArea; // set to 0 in TESObjectWEAP::Fire()
        float fPower; // init to 1.0 and set to 1.0 before launch in TESObjectWEAP::Fire()
        float fScale; // init to 1.0 and set to 1.0 before launch in TESObjectWEAP::Fire()
        bool bAlwaysHit; // usually false
        bool bNoDamageOutsideCombat; // usually false
        bool bAutoAim; // usually true (probably not bAutoAim then?)
        bool bUnkBool2; // usually false
        bool bUseOrigin; // usually false, should be set true for us
        bool bDeferInitialization; // usually false
        bool bForceConeOfFire; // unsure // usually false
    };

    uint8_t unk98[0xF0];
    float fPower;
    float fSpeedMult;
    float fRange;
    float fAge;
    float fDamage;

    static BSPointerHandle<Projectile>* Launch(BSPointerHandle<Projectile>* apResult, LaunchData& arData) noexcept;
};

static_assert(sizeof(Projectile::LaunchData) == 0xA8);

