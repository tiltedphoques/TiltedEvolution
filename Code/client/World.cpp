#include <TiltedOnlinePCH.h>

#include "World.h"

#include <Services/DiscoveryService.h>
#include <Services/EntityService.h>
#include <Services/CharacterService.h>
#include <Services/InputService.h>
#include <Services/OverlayService.h>
#include <Services/TransportService.h>
#include <Services/TestService.h>
#include <Services/RunnerService.h>
#include <Services/ImguiService.h>
#include <Services/ScriptService.h>
#include <Services/PapyrusService.h>
#include <Services/DiscordService.h>
#include <Services/EnvironmentService.h>
#include <Services/QuestService.h>
#include <Services/PartyService.h>
#include <Services/ActorService.h>

#include <Events/PreUpdateEvent.h>
#include <Events/UpdateEvent.h>

World::World()
    : m_runner(m_dispatcher)
    , m_transport(*this, m_dispatcher, set<ImguiService>())
    , m_modSystem(m_dispatcher)
    , m_lastFrameTime{ std::chrono::high_resolution_clock::now() }
{
    set<DiscoveryService>(*this, m_dispatcher);
    set<EntityService>(*this, m_dispatcher);
    set<OverlayService>(*this, m_transport, m_dispatcher);
    set<InputService>(ctx<OverlayService>());
    set<CharacterService>(*this, m_dispatcher, m_transport);
    set<TestService>(m_dispatcher, *this, m_transport, ctx<ImguiService>());
    set<ScriptService>(*this, m_dispatcher, ctx<ImguiService>(), m_transport);
    set<PapyrusService>(m_dispatcher);
    set<DiscordService>(m_dispatcher);
    set<EnvironmentService>(*this, m_dispatcher, ctx<ImguiService>(), m_transport);
    set<QuestService>(*this, m_dispatcher, ctx<ImguiService>());
    set<PartyService>(m_dispatcher, ctx<ImguiService>(), m_transport);
    set<ActorService>(*this, m_dispatcher, m_transport);
}

World::~World() = default;

void World::Update() noexcept
{
    const auto cNow = std::chrono::high_resolution_clock::now();
    const auto cDelta = cNow - m_lastFrameTime;
    m_lastFrameTime = cNow;

    const auto cDeltaSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(cDelta).count();

    m_dispatcher.trigger(PreUpdateEvent(cDeltaSeconds));

    // Force run this before so we get the tasks scheduled to run
    m_runner.OnUpdate(UpdateEvent(cDeltaSeconds));
    m_dispatcher.trigger(UpdateEvent(cDeltaSeconds));
}

RunnerService& World::GetRunner() noexcept
{
    return m_runner;
}

TransportService& World::GetTransport() noexcept
{
    return m_transport;
}

ModSystem& World::GetModSystem() noexcept
{
    return m_modSystem;
}

uint64_t World::GetTick() const noexcept
{
    return m_transport.GetClock().GetCurrentTick();
}

void World::Create() noexcept
{
    if(entt::service_locator<World>::empty())
    {
        entt::service_locator<World>::set(std::make_shared<World>());
    }
}

World& World::Get() noexcept
{
    return entt::service_locator<World>::ref();
}
