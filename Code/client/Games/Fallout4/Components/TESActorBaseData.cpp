#include <TiltedOnlinePCH.h>

#include <Components/TESActorBaseData.h>

TESActorBase* TESActorBaseData::CreateTemplateActorBase(TESActorBase* apOriginalBase, TESActorBase** appTemplates) noexcept
{
    using TCreateTemplateActorBase = decltype(TESActorBaseData::CreateTemplateActorBase);
    static VersionDbPtr<TCreateTemplateActorBase> s_createTemplateActorBase(2192885);
    return s_createTemplateActorBase.Get()(apOriginalBase, appTemplates);
}
