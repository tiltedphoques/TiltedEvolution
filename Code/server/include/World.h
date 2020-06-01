#pragma once

#include <Services/ScriptService.h>
#include <Services/PlayerService.h>
#include <Services/CharacterService.h>

struct World : entt::registry
{
    World();
    ~World() noexcept = default;

    TP_NOCOPYMOVE(World);

    entt::dispatcher& GetDispatcher() noexcept { return m_dispatcher; }
    const entt::dispatcher& GetDispatcher() const noexcept { return m_dispatcher; }
    CharacterService& GetCharacterService() noexcept { return ctx<CharacterService>(); }
    const CharacterService& GetCharacterService() const noexcept { return ctx<CharacterService>(); }
    PlayerService& GetPlayerService() noexcept { return ctx<PlayerService>(); }
    const PlayerService& GetPlayerService() const noexcept { return ctx<PlayerService>(); }
    ScriptService& GetScriptService() noexcept { return m_scriptService; }
    const ScriptService& GetScriptService() const noexcept { return m_scriptService; }

    [[nodiscard]] static uint32_t ToInteger(entt::entity aEntity) { return to_integral(aEntity); }

private:
    entt::dispatcher m_dispatcher;

    ScriptService m_scriptService;
};
