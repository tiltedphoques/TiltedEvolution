#include "Projectile.h"
#include <World.h>
#include <Actor.h>
#include <Games/ActorExtension.h>
#include <Events/ProjectileLaunchedEvent.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESAmmo.h>

TP_THIS_FUNCTION(TLaunch, BSPointerHandle<Projectile>*, BSPointerHandle<Projectile>, ProjectileLaunchData& arData);

static TLaunch* RealLaunch = nullptr;

BSPointerHandle<Projectile>* Projectile::Launch(BSPointerHandle<Projectile>* apResult, ProjectileLaunchData& arData) noexcept
{
    return ThisCall(RealLaunch, apResult, arData);
}

BSPointerHandle<Projectile>* TP_MAKE_THISCALL(HookLaunch, BSPointerHandle<Projectile>, ProjectileLaunchData& arData)
{
    if (arData.pShooter)
    {
        Actor* pActor = Cast<Actor>(arData.pShooter);
        if (pActor)
        {
            ActorExtension* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->IsRemote())
            {
                apThis->handle.iBits = 0;
                return apThis;
            }
        }
    }

    ProjectileLaunchedEvent Event{};
    Event.Origin = arData.Origin;
    if (arData.pProjectileBase)
        Event.ProjectileBaseID = arData.pProjectileBase->formID;
    Event.ZAngle = arData.fZAngle;
    Event.XAngle = arData.fXAngle;
    Event.YAngle = arData.fYAngle;
    if (arData.pShooter)
        Event.ShooterID = arData.pShooter->formID;
    if (arData.pParentCell)
        Event.ParentCellID = arData.pParentCell->formID;
    if (arData.pFromAmmo)
        Event.AmmoID = arData.pFromAmmo->formID;
    Event.Area = arData.iArea;
    Event.Power = arData.fPower;
    Event.Scale = arData.fScale;
    Event.AlwaysHit = arData.bAlwaysHit;
    Event.ConeOfFireRadiusMult = arData.fConeOfFireRadiusMult;
    Event.NoDamageOutsideCombat = arData.bNoDamageOutsideCombat;
    Event.AutoAim = arData.bAutoAim;
    Event.DeferInitialization = arData.bDeferInitialization;
    Event.Tracer = arData.bTracer;
    Event.ForceConeOfFire = arData.bForceConeOfFire;
    Event.IntentionalMiss = arData.bIntentionalMiss;
    Event.Allow3D = arData.bAllow3D;
    Event.Penetrates = arData.bPenetrates;
    Event.IgnoreNearCollisions = arData.bIgnoreNearCollisions;

    World::Get().GetRunner().Trigger(Event);

    return ThisCall(RealLaunch, apThis, arData);
}

TP_THIS_FUNCTION(TFire, void, void, TESObjectREFR* apSource, uint32_t aEquipIndex, TESAmmo* apAmmo, AlchemyItem* apPoison);
static TFire* RealFire = nullptr;

void TP_MAKE_THISCALL(HookFire, void, TESObjectREFR* apSource, uint32_t aEquipIndex, TESAmmo* apAmmo, AlchemyItem* apPoison)
{
    if (apSource)
    {
        Actor* pActor = Cast<Actor>(apSource);
        if (pActor)
        {
            ActorExtension* pExtension = pActor->GetExtension();
            if (pExtension)
            {
                if (pExtension->IsRemote())
                    return;
            }
        }
    }

    return ThisCall(RealFire, apThis, apSource, aEquipIndex, apAmmo, apPoison);
}

static TiltedPhoques::Initializer s_projectileHooks([]() {
    POINTER_FALLOUT4(TLaunch, s_launch, 1452335);
    POINTER_FALLOUT4(TFire, fire, 1056038);

    RealLaunch = s_launch.Get();
    RealFire = fire.Get();

    TP_HOOK(&RealLaunch, HookLaunch);
    TP_HOOK(&RealFire, HookFire);
});

