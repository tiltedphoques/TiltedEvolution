#include <Services/CommandService.h>

#include <Events/CommandEvent.h>

CommandService::CommandService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher) 
    : m_world(aWorld), 
      m_transport(aTransport)
{
    m_commandEventConnection = aDispatcher.sink<CommandEvent>().connect<&CommandService::OnCommandEvent>(this));
}

void CommandService::OnCommandEvent(const CommandEvent& acEvent) noexcept
{

}
