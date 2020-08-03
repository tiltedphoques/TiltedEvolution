#pragma once

#include <Games/Skyrim/Misc/SkyrimVM.h>
#include <Games/Fallout4/Misc/GameVM.h>

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

template<class Return, class Type, class... Args>
struct PapyrusFunction
{
    using TFunction = Return(__fastcall*)(BSScript::IVirtualMachine*, uint32_t, const Type*, Args...);

    PapyrusFunction(const void* apAddress)
        : m_pFunction(reinterpret_cast<TFunction>(apAddress))
    {}

    Return operator()(const Type* apThis, Args... args) const noexcept
    {
        return m_pFunction(GameVM::Get()->virtualMachine, 0, apThis, std::forward<Args>(args) ...);
    }

private:

    TFunction m_pFunction;
};

#define PAPYRUS_FUNCTION(returnType, scope, name, ...) static PapyrusFunction<returnType, scope, __VA_ARGS__> s_p##name(World::Get().ctx<PapyrusService>().Get(#scope, #name));
