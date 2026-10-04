#include <AI/AIProcess.h>

void AIProcess::KnockExplosion(Actor* apActor, const NiPoint3* aSourceLocation, float afMagnitude) noexcept
{
    TP_THIS_FUNCTION(TKnockExplosion, void, AIProcess, Actor*, const NiPoint3&, float);
    static VersionDbPtr<TKnockExplosion> knockExplosion(2232384);
    TiltedPhoques::ThisCall(knockExplosion, this, apActor, *aSourceLocation, afMagnitude);
}
