#include "Projectile.h"
#include <Games/Skyrim/Forms/TESObjectWEAP.h>
#include <Games/Skyrim/Forms/MagicItem.h>
#include <Games/Skyrim/Forms/TESAmmo.h>
#include <Actor.h>
#include <Games/ActorExtension.h>
#include <World.h>
#include <Events/ProjectileLaunchedEvent.h>
#include <Games/Skyrim/Forms/TESObjectCELL.h>
#include <Forms/SpellItem.h>

TP_THIS_FUNCTION(TLaunch, uint32_t*, void, Projectile::LaunchData& arData);
static TLaunch* RealLaunch = nullptr;

uint32_t* Projectile::Launch(void* apResult, LaunchData& apLaunchData) noexcept
{
    auto result = ThisCall(RealLaunch, apResult, apLaunchData);

    TP_ASSERT(result, "No projectile handle returned.");

    auto* pObject = TESObjectREFR::GetByHandle(*result);
    auto* pProjectile = RTTI_CAST(pObject, TESObjectREFR, Projectile);
    
    TP_ASSERT(pProjectile, "No projectile found.");

    pProjectile->fPower = apLaunchData.fPower;

    return result;
}

uint32_t* TP_MAKE_THISCALL(HookLaunch, void, Projectile::LaunchData& arData)
{
    // sync concentration spells through spell cast sync, the rest through projectile sync
    if (arData.pSpell)
    {
        if (auto* pSpell = RTTI_CAST(arData.pSpell, MagicItem, SpellItem))
        {
            if (pSpell->eCastingType == MagicSystem::CastingType::CONCENTRATION)
            {
                return ThisCall(RealLaunch, apThis, arData);
            }
        }
    }

    if (arData.pShooter)
    {
        auto* pActor = RTTI_CAST(arData.pShooter, TESObjectREFR, Actor);
        if (pActor)
        {
            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->IsRemote())
                return nullptr;
        }
    }

    ProjectileLaunchedEvent Event{};
    Event.Origin = arData.Origin;
    if (arData.pProjectileBase)
        Event.ProjectileBaseID = arData.pProjectileBase->formID;
    if (arData.pShooter)
        Event.ShooterID = arData.pShooter->formID;
    if (arData.pFromWeapon)
        Event.WeaponID = arData.pFromWeapon->formID;
    if (arData.pFromAmmo)
        Event.AmmoID = arData.pFromAmmo->formID;
    Event.ZAngle = arData.fZAngle;
    Event.XAngle = arData.fXAngle;
    Event.YAngle = arData.fYAngle;
    if (arData.pParentCell)
        Event.ParentCellID = arData.pParentCell->formID;
    if (arData.pSpell)
        Event.SpellID = arData.pSpell->formID;
    Event.CastingSource = arData.eCastingSource;
    Event.unkBool1 = arData.unkBool1;
    Event.Area = arData.iArea;
    Event.Power = arData.fPower;
    Event.Scale = arData.fScale;
    Event.AlwaysHit = arData.bAlwaysHit;
    Event.NoDamageOutsideCombat = arData.bNoDamageOutsideCombat;
    Event.AutoAim = arData.bAutoAim;
    /*
    Event.UseOrigin = arData.bUseOrigin;
    Event.DeferInitialization = arData.bDeferInitialization;
    */
    Event.Tracer = arData.bTracer;
    Event.ForceConeOfFire = arData.bForceConeOfFire;

    auto result = ThisCall(RealLaunch, apThis, arData);

    TP_ASSERT(result, "No projectile handle returned.");

    auto* pObject = TESObjectREFR::GetByHandle(*result);
    auto* pProjectile = RTTI_CAST(pObject, TESObjectREFR, Projectile);
    
    TP_ASSERT(pProjectile, "No projectile found.");

    Event.Power = pProjectile->fPower;

    World::Get().GetRunner().Trigger(Event);

    return result;
}

static TiltedPhoques::Initializer s_projectileHooks([]() {
    POINTER_SKYRIMSE(TLaunch, s_launch, 0x140778480 - 0x140000000);

    RealLaunch = s_launch.Get();

    TP_HOOK(&RealLaunch, HookLaunch);
});

