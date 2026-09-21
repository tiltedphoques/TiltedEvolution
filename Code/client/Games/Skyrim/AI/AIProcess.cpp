#include "AIProcess.h"

bhkCharacterController* AIProcess::GetCharController() noexcept
{
    TP_THIS_FUNCTION(TGetCharController, bhkCharacterController*, AIProcess);
    POINTER_SKYRIMSE(TGetCharController, getCharController, 39856);
    return TiltedPhoques::ThisCall(getCharController, this);
}

void AIProcess::KnockExplosion(Actor* apActor, const NiPoint3* aSourceLocation, float afMagnitude)
{
    TP_THIS_FUNCTION(TKnockExplosion, void, AIProcess, Actor*, const NiPoint3*, float);
    POINTER_SKYRIMSE(TKnockExplosion, knockExplosion, 39895);
    TiltedPhoques::ThisCall(knockExplosion, this, apActor, aSourceLocation, afMagnitude);
}
