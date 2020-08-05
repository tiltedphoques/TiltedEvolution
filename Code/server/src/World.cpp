#include <World.h>
#include <Components.h>

#include <Services/CharacterService.h>
#include <Services/PlayerService.h>
#include <Services/EnvironmentService.h>

World::World()
    : m_scriptService(*this, m_dispatcher)
{
    set<CharacterService>(*this, m_dispatcher);
    set<PlayerService>(*this, m_dispatcher);
    set<EnvironmentService>(*this, m_dispatcher);
    set<ModsComponent>();
}
