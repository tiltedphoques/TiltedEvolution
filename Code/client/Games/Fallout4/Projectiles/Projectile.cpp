#include "Projectile.h"
#include <World.h>
#include <Actor.h>
#include <Games/ActorExtension.h>
#include <Events/ProjectileLaunchedEvent.h>
#include <Forms/TESObjectCELL.h>

TP_THIS_FUNCTION(TLaunch, void*, void, ProjectileLaunchData& arData);

static TLaunch* RealLaunch = nullptr;

void* Projectile::Launch(void* apResult, ProjectileLaunchData& arData)
{
    return ThisCall(RealLaunch, apResult, arData);
}

void* TP_MAKE_THISCALL(HookLaunch, void, ProjectileLaunchData& arData)
{
    static uint64_t s_launchCount = 0;
    s_launchCount++;
    spdlog::warn("Launch count: {}", s_launchCount);

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

    spdlog::info("\tOrigin: {}, {}, {}", arData.Origin.x, arData.Origin.y, arData.Origin.z);
    spdlog::info("\tContactNormal: {}, {}, {}", arData.ContactNormal.x, arData.ContactNormal.y, arData.ContactNormal.z);

    ProjectileLaunchedEvent Event;
    Event.Origin = arData.Origin;
    Event.ContactNormal = arData.ContactNormal;
    if (arData.pProjectileBase)
        Event.ProjectileBaseID = arData.pProjectileBase->formID;
    Event.ZAngle = arData.fZAngle;
    Event.XAngle = arData.fXAngle;
    Event.YAngle = arData.fYAngle;
    if (arData.pParentCell)
        Event.ParentCellID = arData.pParentCell->formID;
    Event.Area = arData.iArea;
    Event.Power = arData.fPower;
    Event.Scale = arData.fScale;
    Event.AlwaysHit = arData.bAlwaysHit;
    Event.ConeOfFireRadiusMult = arData.fConeOfFireRadiusMult;
    Event.NoDamageOutsideCombat = arData.bNoDamageOutsideCombat;
    Event.AutoAim = arData.bAutoAim;
    Event.UseOrigin = arData.bUseOrigin;
    Event.DeferInitialization = arData.bDeferInitialization;
    Event.Tracer = arData.bTracer;
    Event.ForceConeOfFire = arData.bForceConeOfFire;
    Event.IntentionalMiss = arData.bIntentionalMiss;
    Event.Allow3D = arData.bAllow3D;
    Event.Penetrates = arData.bPenetrates;
    Event.IgnoreNearCollisions = arData.bIgnoreNearCollisions;

    //World::Get().GetRunner().Trigger(Event);

    return ThisCall(RealLaunch, apThis, arData);
}

static TiltedPhoques::Initializer s_projectileHooks([]() {
    POINTER_FALLOUT4(TLaunch, s_launch, 0x140FCA260 - 0x140000000);

    RealLaunch = s_launch.Get();

    TP_HOOK(&RealLaunch, HookLaunch);
});

