#pragma once

#include <Games/Fallout4/TESObjectREFR.h>
#include <Games/Magic/MagicSystem.h>

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
    // ProjectileLaunchData, defaults as set by its constructor.
    struct LaunchData
    {
        NiPoint3 Origin{};
        NiPoint3 ContactNormal{};
        TESForm* pProjectileBase = nullptr; // BGSProjectile
        TESObjectREFR* pShooter = nullptr;
        CombatController* pCombatController = nullptr;
        TESObjectWEAP* pFromWeapon = nullptr; // BGSObjectInstanceT<TESObjectWEAP>
        void* pFromWeaponInstanceData = nullptr;
        TESAmmo* pFromAmmo = nullptr;
        uint32_t EquipIndex = 0xFFFFFFFF;
        float fZAngle = 0.f;
        float fXAngle = 0.f;
        float fYAngle = 0.f;
        TESObjectREFR* pHomingTarget = nullptr;
        TESObjectCELL* pParentCell = nullptr;
        MagicItem* pSpell = nullptr;
        MagicSystem::CastingSource eCastingSource = MagicSystem::CastingSource::CASTING_SOURCE_COUNT;
        AlchemyItem* pPoison = nullptr;
        int32_t iArea = 0;
        float fPower = 1.f;
        float fScale = 1.f;
        float fConeOfFireRadiusMult = 1.f;
        int32_t eTargetLimb = -1;
        bool bAlwaysHit = false;
        bool bNoDamageOutsideCombat = false;
        bool bAutoAim = false;
        bool bUseOrigin = false;
        bool bDeferInitialization = false;
        bool bTracer = false;
        bool bForceConeOfFire = false;
        bool bIntentionalMiss = false;
        bool bAllow3D = true;
        bool bPenetrates = false;
        bool bIgnoreNearCollisions = false;
    };

    static BSPointerHandle<Projectile>* Launch(BSPointerHandle<Projectile>* apResult, LaunchData& arData) noexcept;
};

static_assert(offsetof(Projectile::LaunchData, pFromWeapon) == 0x30);
static_assert(offsetof(Projectile::LaunchData, EquipIndex) == 0x48);
static_assert(offsetof(Projectile::LaunchData, pParentCell) == 0x60);
static_assert(offsetof(Projectile::LaunchData, pPoison) == 0x78);
static_assert(offsetof(Projectile::LaunchData, bAlwaysHit) == 0x94);
static_assert(offsetof(Projectile::LaunchData, bAllow3D) == 0x9C);
static_assert(sizeof(Projectile::LaunchData) == 0xA0);
