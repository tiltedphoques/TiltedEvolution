#include <TiltedOnlinePCH.h>

#include <Events/PapyrusFunctionRegisterEvent.h>

#include <Services/PapyrusService.h>

PapyrusService::PapyrusService(entt::dispatcher& aDispatcher) noexcept
{
    m_papyrusFunctionRegisterConnection = aDispatcher.sink<PapyrusFunctionRegisterEvent>().connect<&PapyrusService::HandlePapyrusFunctionEvent>(this);
}

const void* PapyrusService::Get(const String& acNamespace, const String& acFunction) const noexcept
{
    const auto itor = m_functions.find(acNamespace + "::" + acFunction);
    if (itor != std::end(m_functions))
        return itor->second;

    return nullptr;
}

void PapyrusService::HandlePapyrusFunctionEvent(const PapyrusFunctionRegisterEvent& acEvent) noexcept
{
    m_functions[acEvent.Namespace + "::" + acEvent.Name] = acEvent.Function;
}
