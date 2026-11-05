#include <AI/AIProcess.h>

bhkCharacterController* AIProcess::GetCharController() noexcept
{
    TP_THIS_FUNCTION(TGetCharController, bhkCharacterController*, AIProcess);
    static VersionDbPtr<TGetCharController> getCharController(2232351);
    return TiltedPhoques::ThisCall(getCharController, this);
}

void AIProcess::KnockExplosion(Actor* apActor, const NiPoint3* aSourceLocation, float afMagnitude) noexcept
{
    TP_THIS_FUNCTION(TKnockExplosion, void, AIProcess, Actor*, const NiPoint3&, float);
    static VersionDbPtr<TKnockExplosion> knockExplosion(2232384);
    TiltedPhoques::ThisCall(knockExplosion, this, apActor, *aSourceLocation, afMagnitude);
}

// AIProcess::SetLookAt
void AIProcess::SetHeadTrackTarget(Actor* apOwner, const NiPoint3& acTarget) noexcept
{
    TP_THIS_FUNCTION(TSetLookAt, void, AIProcess, Actor*, const NiPoint3&);
    static VersionDbPtr<TSetLookAt> setLookAt(2231871);
    TiltedPhoques::ThisCall(setLookAt, this, apOwner, acTarget);
}
