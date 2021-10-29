#include "Projectile.h"
#include <Games/Skyrim/Forms/TESObjectWEAP.h>
#include <Games/Skyrim/Forms/MagicItem.h>
#include <Games/Skyrim/Forms/TESAmmo.h>
#include <Actor.h>
#include <Games/ActorExtension.h>
#include <World.h>
#include <Events/ProjectileLaunchedEvent.h>
#include <Games/Skyrim/Forms/TESObjectCELL.h>

TP_THIS_FUNCTION(TLaunch, void*, void, Projectile::LaunchData* arData);

static TLaunch* RealLaunch = nullptr;

void* TP_MAKE_THISCALL(HookLaunch, void, Projectile::LaunchData* arData)
{
    if (!arData->pFromAmmo)
    {
        return ThisCall(RealLaunch, apThis, arData);
    }

    if (arData->pShooter)
    {
        auto* pActor = RTTI_CAST(arData->pShooter, TESObjectREFR, Actor);
        if (pActor)
        {
            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->IsRemote())
                return nullptr;
        }
    }

    spdlog::info("Projectile launched, data:");
    spdlog::info("\tOrigin: {}, {}, {}", arData->Origin.x, arData->Origin.y, arData->Origin.z);
    spdlog::info("\tContactNormal: {}, {}, {}", arData->ContactNormal.x, arData->ContactNormal.y, arData->ContactNormal.z);
    spdlog::info("\tShooter form id: {:X}", arData->pShooter ? arData->pShooter->formID : 0);
    spdlog::info("\tWeapon form id: {:X}", arData->pFromWeapon ? arData->pFromWeapon->formID : 0);
    spdlog::info("\tAmmo form id: {:X}", arData->pFromAmmo ? arData->pFromAmmo->formID : 0);
    spdlog::info("\tAngles: z: {}, x: {}, y: {}", arData->fZAngle, arData->fXAngle, arData->fYAngle);
    spdlog::info("\tSpell form id: {:X}", arData->pSpell ? arData->pSpell->formID : 0);
    spdlog::info("\tUse origin: {}", arData->bUseOrigin);
    spdlog::info("\tProjectile base form id: {:X}", arData->pProjectileBase ? arData->pProjectileBase->formID : 0);

    ProjectileLaunchedEvent Event;
    Event.Origin = arData->Origin;
    Event.ContactNormal = arData->ContactNormal;
    Event.ProjectileBaseID = arData->pProjectileBase->formID;
    Event.ShooterID = arData->pShooter->formID;
    Event.WeaponID = arData->pFromWeapon->formID;
    Event.AmmoID = arData->pFromAmmo->formID;
    Event.ZAngle = arData->fZAngle;
    Event.XAngle = arData->fXAngle;
    Event.YAngle = arData->fYAngle;
    Event.ParentCellID = arData->pParentCell->formID;
    Event.unkBool1 = arData->unkBool1;
    Event.Area = arData->iArea;
    Event.Power = arData->fPower;
    Event.Scale = arData->fScale;
    Event.AlwaysHit = arData->bAlwaysHit;
    Event.NoDamageOutsideCombat = arData->bNoDamageOutsideCombat;
    Event.AutoAim = arData->bAutoAim;
    Event.UseOrigin = arData->bUseOrigin;
    Event.DeferInitialization = arData->bDeferInitialization;
    Event.Tracer = arData->bTracer;
    Event.ForceConeOfFire = arData->bForceConeOfFire;

    World::Get().GetRunner().Trigger(Event);

    return ThisCall(RealLaunch, apThis, arData);
}

void* Projectile::Launch(void* apResult, LaunchData* apLaunchData) noexcept
{
    POINTER_SKYRIMSE(TLaunch, s_launch, 0x14074B170 - 0x140000000);

    return ThisCall(s_launch, apResult, apLaunchData);
}

static TiltedPhoques::Initializer s_projectileHooks([]() {
    POINTER_SKYRIMSE(TLaunch, s_launch, 0x14074B170 - 0x140000000);

    RealLaunch = s_launch.Get();

    TP_HOOK(&RealLaunch, HookLaunch);
});

