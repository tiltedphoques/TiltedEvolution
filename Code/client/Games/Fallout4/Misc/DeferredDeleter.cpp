#include <TiltedOnlinePCH.h>

#include <Misc/DeferredDeleter.h>

void DeferredDeleter::Add(TESBoundObject* apObject) noexcept
{
    using TAdd = void(TESBoundObject*);
    static VersionDbPtr<TAdd> s_add(2228657);
    s_add.Get()(apObject);
}
