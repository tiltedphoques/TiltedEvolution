#include <TiltedOnlinePCH.h>

#include "World.h"

#include <Services/DiscoveryService.h>
#include <Services/InputService.h>
#include <Services/TransportService.h>
#include <Services/RunnerService.h>
#include <Services/ImguiService.h>
#include <Services/PapyrusService.h>
#include <Services/DiscordService.h>
#include <Services/ObjectService.h>
#include <Services/QuestService.h>
#include <Services/ActorValueService.h>
#include <Services/InventoryService.h>
#include <Services/MagicService.h>
#include <Services/CommandService.h>
#include <Services/CalendarService.h>
#include <Services/StringCacheService.h>
#include <Services/PlayerService.h>
#include <Services/CombatService.h>
#include <Services/WeatherService.h>

#include <Events/PreUpdateEvent.h>
#include <Events/UpdateEvent.h>

World::World()
    : m_runner(m_dispatcher)
    , m_transport(*this, m_dispatcher)
    , m_modSystem(m_dispatcher)
    , m_lastFrameTime{ std::chrono::high_resolution_clock::now() }
{
     ctx().emplace<ImguiService>();
     ctx().emplace<DiscoveryService>(*this, m_dispatcher);
     ctx().emplace<OverlayService>(*this, m_transport, m_dispatcher);
     ctx().emplace<InputService>(ctx().at<OverlayService>());
     ctx().emplace<CharacterService>(*this, m_dispatcher, m_transport);
     ctx().emplace<DebugService>(m_dispatcher, *this, m_transport, ctx().at<ImguiService>());
     ctx().emplace<PapyrusService>(m_dispatcher);
     ctx().emplace<DiscordService>(m_dispatcher);
     ctx().emplace<ObjectService>(*this, m_dispatcher, m_transport);
     ctx().emplace<CalendarService>(*this, m_dispatcher, m_transport);
     ctx().emplace<QuestService>(*this, m_dispatcher);
     ctx().emplace<PartyService>(*this, m_dispatcher, m_transport);
     ctx().emplace<ActorValueService>(*this, m_dispatcher, m_transport);
     ctx().emplace<InventoryService>(*this, m_dispatcher, m_transport);
     ctx().emplace<MagicService>(*this, m_dispatcher, m_transport);
     ctx().emplace<CommandService>(*this, m_transport, m_dispatcher);
     ctx().emplace<PlayerService>(*this, m_dispatcher, m_transport);
     ctx().emplace<StringCacheService>(m_dispatcher);
     ctx().emplace<CombatService>(*this, m_transport, m_dispatcher);
     ctx().emplace<WeatherService>(*this, m_transport, m_dispatcher);
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
    if (!entt::locator<World>::has_value())
    {
        entt::locator<World>::emplace();
    }
}

World& World::Get() noexcept
{
    return entt::locator<World>::value();
}
