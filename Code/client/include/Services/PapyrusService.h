#pragma once

struct PapyrusFunctionRegisterEvent;

struct PapyrusService
{
    PapyrusService(entt::dispatcher& aDispatcher) noexcept;
    ~PapyrusService() noexcept = default;

    TP_NOCOPYMOVE(PapyrusService);

    const void* Get(const String& acNamespace, const String& acFunction) const noexcept;

    void HandlePapyrusFunctionEvent(const PapyrusFunctionRegisterEvent&) noexcept;

private:

    Map<String, void*> m_functions;

    entt::scoped_connection m_papyrusFunctionRegisterConnection;  
};

#define PAPYRUS_FUNCTION(returnType, scope, name, ...) using T##scope_##name = returnType(BSScript::IVirtualMachine*, uint32_t, scope*, __VA_ARGS__); \
    static auto s_p##name = reinterpret_cast<T##scope_##name*>(World::Get().ctx<PapyrusService>().Get(#scope, #name));
