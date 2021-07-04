#include "MagicCaster.h"

#include <Events/FireProjectileEvent.h>

#include <World.h>

TP_THIS_FUNCTION(TFireProjectile, void*, MagicCaster);

TFireProjectile* RealFireProjectile = nullptr;

void* MagicCaster::FireProjectile() noexcept
{
    spdlog::info("Calling fire projectile");
    return ThisCall(RealFireProjectile, this);
}

void* TP_MAKE_THISCALL(FireProjectileHook, MagicCaster)
{
    //World::Get().GetRunner().Trigger(FireProjectileEvent(apThis->pCastingActor));

    return ThisCall(RealFireProjectile, apThis);
}

static TiltedPhoques::Initializer s_magicCasterHooks([]() 
    {
        POINTER_SKYRIMSE(TFireProjectile, s_fireProjectile, 0x14054FF80 - 0x140000000);

        RealFireProjectile = s_fireProjectile.Get();

        TP_HOOK(&RealFireProjectile, FireProjectileHook);
    });
