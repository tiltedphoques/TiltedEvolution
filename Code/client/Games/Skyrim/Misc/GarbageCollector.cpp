#include <TiltedOnlinePCH.h>

#include <Misc/GarbageCollector.h>

GarbageCollector* GarbageCollector::Get() noexcept
{
    POINTER_SKYRIMSE(GarbageCollector*, s_singleton, 400329);
    return *s_singleton.Get();
}

void GarbageCollector::Add(TESBoundObject* apObject) noexcept
{
    // The base-object overload used by Actor::RecalcLeveledActor (among other functions);
    TP_THIS_FUNCTION(TAdd, void, GarbageCollector, TESBoundObject*);
    POINTER_SKYRIMSE(TAdd, s_add, 36460);
    TiltedPhoques::ThisCall(s_add, this, apObject);
}
