#pragma once

#include <Forms/TESBoundObject.h>
#include <Games/Skyrim/Misc/MagicSystem.h>

struct BGSProjectile;
struct TESObjectREFR;
struct TESObjectWEAP;
struct TESAmmo;
struct TESObjectCELL;
struct MagicItem;
struct AlchemyItem;

struct ProjectileLaunchData
{
    NiPoint3 Origin;
    NiPoint3 ContactNormal;
    uint8_t unk18[0x8];
    BGSProjectile* pProjectileBase;
    TESObjectREFR* pShooter;
    uint8_t unk30[0x8];
    TESObjectWEAP* pFromWeapon;
    TESAmmo* pFromAmmo;
    float fZAngle;
    float fXAngle;
    float fYAngle;
    uint8_t unk54[0x14];
    TESObjectCELL* pParentCell;
    MagicItem* pSpell;
    MagicSystem::CastingSource eCastingSource;
    uint8_t unk7C[0x8];
    AlchemyItem* pPoison;
    uint8_t unk90[0x4];
    float fPower;
    float fScale;
    float fConeOfFireRadiusMult; // might not be cone of fire?
    bool bUseOrigin;
};

static_assert(offsetof(ProjectileLaunchData, fConeOfFireRadiusMult) == 0x9C);
static_assert(offsetof(ProjectileLaunchData, bUseOrigin) == 0xA0);

struct TESObjectWEAP : TESBoundObject
{
};
