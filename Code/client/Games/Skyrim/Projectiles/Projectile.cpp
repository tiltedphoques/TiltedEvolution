#include "Projectile.h"
#include <Games/Skyrim/Forms/TESObjectWEAP.h>
#include <Games/Skyrim/Forms/MagicItem.h>
#include <Games/Skyrim/Forms/TESAmmo.h>
#include <Actor.h>
#include <Games/ActorExtension.h>
#include <World.h>

TP_THIS_FUNCTION(TLaunch, void*, void, Projectile::LaunchData* arData);

static TLaunch* RealLaunch = nullptr;

void* TP_MAKE_THISCALL(HookLaunch, void, Projectile::LaunchData* arData)
{
    if (arData->pShooter)
    {
        auto* pActor = RTTI_CAST(arData->pShooter, TESObjectREFR, Actor);
        if (pActor)
        {
            auto* pExtendedActor = pActor->GetExtension();
            /*
            if (pExtendedActor->IsRemote())
                return nullptr;
            */
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

    //World::Get().GetRunner().Trigger()

    return ThisCall(RealLaunch, apThis, arData);
}

static TiltedPhoques::Initializer s_projectileHooks([]() {
    POINTER_SKYRIMSE(TLaunch, s_launch, 0x14074B170 - 0x140000000);

    RealLaunch = s_launch.Get();

    TP_HOOK(&RealLaunch, HookLaunch);
});

