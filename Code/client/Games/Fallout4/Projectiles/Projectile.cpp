#include "Projectile.h"
#include <World.h>
#include <Actor.h>
#include <Games/ActorExtension.h>
#include <Events/ProjectileLaunchedEvent.h>

TP_THIS_FUNCTION(TLaunch, void*, void, ProjectileLaunchData& arData);

static TLaunch* RealLaunch = nullptr;

void* Projectile::Launch(void* apResult, ProjectileLaunchData& arData)
{
    return ThisCall(RealLaunch, apResult, arData);
}

void* TP_MAKE_THISCALL(HookLaunch, void, ProjectileLaunchData& arData)
{
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

    ProjectileLaunchedEvent Event;
    Event.Origin = arData.Origin;
    Event.ContactNormal = arData.ContactNormal;
    if (arData.pProjectileBase)
        Event.ProjectileBaseID = arData.pProjectileBase->formID;
    if (arData.pShooter)
        Event.ShooterID = arData.pShooter->formID;
    //if (arData.pFromWeapon)

    World::Get().GetRunner().Trigger(Event);

    return ThisCall(RealLaunch, apThis, arData);
}

static TiltedPhoques::Initializer s_projectileHooks([]() {
    POINTER_FALLOUT4(TLaunch, s_launch, 0x140FCA250 - 0x140000000);

    RealLaunch = s_launch.Get();

    TP_HOOK(&RealLaunch, HookLaunch);
});

