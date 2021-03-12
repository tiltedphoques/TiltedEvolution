#include <stdafx.h>

#include <World.h>
#include <Components.h>

#include <Services/CharacterService.h>
#include <Services/PlayerService.h>
#include <Services/EnvironmentService.h>
#include <Services/QuestService.h>
#include <Services/ServerListService.h>
#include <Services/PartyService.h>
#include <Services/ActorService.h>

World::World()
{
    set<CharacterService>(*this, m_dispatcher);
    set<PlayerService>(*this, m_dispatcher);
    set<EnvironmentService>(*this, m_dispatcher);
    set<ModsComponent>();
    set<ServerListService>(*this, m_dispatcher);
    set<QuestService>(*this, m_dispatcher);
    set<PartyService>(*this, m_dispatcher);
    set<ActorService>(*this, m_dispatcher);

    // late initialize the ScriptService to ensure all components are valid
    m_scriptService = std::make_unique<ScriptService>(*this, m_dispatcher);
}
