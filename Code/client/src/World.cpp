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
#include <Services/EventListenService.h>

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
    set<OverlayService>();
    set<InputService>(set<OverlayService>());
    set<CharacterService>(*this, m_dispatcher, m_transport);
    set<TestService>(m_dispatcher, *this, m_transport, ctx<ImguiService>());
    set<ScriptService>(*this, m_dispatcher, ctx<ImguiService>(), m_transport);
    set<PapyrusService>(m_dispatcher);
    set<DiscordService>(m_dispatcher);
    set<EnvironmentService>(*this, m_dispatcher);
    set<QuestService>(*this, m_dispatcher, ctx<ImguiService>());
    set<PartyService>(m_dispatcher, ctx<ImguiService>(), m_transport);
    set<EventListenService<UnknownEvent>>(&EventDispatcherManager::unknownDispatcher8, "Dispatcher8");

    //set<EventListenService<BGSEventProcessedEvent>>(&EventDispatcherManager::eventProcessedEvent, "eventProcessedEvent");
    //set<EventListenService<TESActivateEvent>>(&EventDispatcherManager::activateEvent, "activateEvent");
    //set<EventListenService<TESActiveEffectApplyRemove>>(&EventDispatcherManager::activeEffectApplyRemove, "activeEffectApplyRemove");
    //set<EventListenService<TESActorLocationChangeEvent>>(&EventDispatcherManager::actorLocationChangeEvent, "actorLocationChangeEvent");
    //set<EventListenService<TESBookReadEvent>>(&EventDispatcherManager::bookReadEvent, "bookReadEvent");
    //set<EventListenService<TESCellAttachDetachEvent>>(&EventDispatcherManager::cellAttachDetachEvent, "cellAttachDetachEvent");
    //set<EventListenService<TESCellFullyLoadedEvent>>(&EventDispatcherManager::cellFullyLoadedEvent, "cellFullyLoadedEvent");
    //set<EventListenService<UnknownEvent>>(&EventDispatcherManager::unknownDispatcher8, "unknownDispatcher8"); // apply decals event
    //set<EventListenService<TESCombatEvent>>(&EventDispatcherManager::combatEvent, "combatEvent");
    set<EventListenService<TESContainerChangedEvent>>(&EventDispatcherManager::containerChangedEvent, "containerChangedEvent");
    //set<EventListenService<TESDeathEvent>>(&EventDispatcherManager::deathEvent, "deathEvent");
    //set<EventListenService<TESDestructionStageChangedEvent>>(&EventDispatcherManager::destructionStageChangedEventDispatcher, "destructionStageChangedEventDispatcher");
    //set<EventListenService<TESEnterBleedoutEvent>>(&EventDispatcherManager::enterBleedoutEvent, "enterBleedoutEvent");
    //set<EventListenService<TESEquipEvent>>(&EventDispatcherManager::equipEvent, "equipEvent");
    //set<EventListenService<TESFormDeleteEvent>>(&EventDispatcherManager::formDeleteEvent, "formDeleteEvent");
    //set<EventListenService<TESFurnitureEvent>>(&EventDispatcherManager::furnitureEvent, "furnitureEvent");
    //set<EventListenService<TESGrabReleaseEvent>>(&EventDispatcherManager::grabReleaseEvent, "grabReleaseEvent");
    //set<EventListenService<TESHitEvent>>(&EventDispatcherManager::hitEvent, "hitEvent"); // validated
    //set<EventListenService<TESInitScriptEvent>>(&EventDispatcherManager::initScriptEvent, "initScriptEvent");
    //set<EventListenService<TESLoadGameEvent>>(&EventDispatcherManager::loadGameEvent, "loadGameEvent");
    //set<EventListenService<TESLockChangedEvent>>(&EventDispatcherManager::lockChangedEvent, "lockChangedEvent");
    //set<EventListenService<TESMagicEffectApplyEvent>>(&EventDispatcherManager::magicEffectApplyEvent, "magicEffectApplyEvent");
    //set<EventListenService<TESMagicWardHitEvent>>(&EventDispatcherManager::magicWardHitEvent, "magicWardHitEvent");
    //set<EventListenService<TESMoveAttachDetachEvent>>(&EventDispatcherManager::moveAttachDetachEvent, "moveAttachDetachEvent");
    //set<EventListenService<TESObjectLoadedEvent>>(&EventDispatcherManager::objectLoadedEvent, "objectLoadedEvent");
    //set<EventListenService<TESObjectREFRTranslationEvent>>(&EventDispatcherManager::objectREFRTranslationEvent, "objectREFRTranslationEvent");
    //set<EventListenService<TESOpenCloseEvent>>(&EventDispatcherManager::openCloseEvent, "openCloseEvent");
    //set<EventListenService<TESPackageEvent>>(&EventDispatcherManager::packageEvent, "packageEvent");
    //set<EventListenService<TESPerkEntryRunEvent>>(&EventDispatcherManager::perkEntryRunEvent, "perkEntryRunEvent");
    //set<EventListenService<TESQuestInitEvent>>(&EventDispatcherManager::questInitEvent, "questInitEvent"); // 9F8
    //set<EventListenService<TESQuestStageEvent>>(&EventDispatcherManager::questStageEvent, "questStageEvent");
    //set<EventListenService<TESQuestStageItemDoneEvent>>(&EventDispatcherManager::questStageItemDoneEvent, "questStageItemDoneEvent"); // AA8, validated StageItemFinishedCallback::TriggerItemDoneEvent
    //set<EventListenService<TESQuestStartStopEvent>>(&EventDispatcherManager::questStartStopEvent, "questStartStopEvent"); // TESResolveNPCTemplatesEvent
    //set<EventListenService<TESResetEvent>>(&EventDispatcherManager::resetEvent, "resetEvent");                   // validated 0xB58
    //set<EventListenService<TESResolveNPCTemplatesEvent>>(&EventDispatcherManager::resolveNPCTemplatesEvent, "resolveNPCTemplatesEvent");
    //set<EventListenService<TESSceneEvent>>(&EventDispatcherManager::sceneEvent, "sceneEvent");
    //set<EventListenService<TESSceneActionEvent>>(&EventDispatcherManager::sceneActionEvent, "sceneActionEvent");
    //set<EventListenService<TESScenePhaseEvent>>(&EventDispatcherManager::scenePhaseEvent, "scenePhaseEvent");
    //set<EventListenService<TESSellEvent>>(&EventDispatcherManager::sellEvent, "sellEvent");
    //set<EventListenService<TESSleepStartEvent>>(&EventDispatcherManager::unknownDispatcher39, "unknownDispatcher39");
    //set<EventListenService<TESSleepStopEvent>>(&EventDispatcherManager::sleepStopEvent, "sleepStopEvent");
    //set<EventListenService<TESSpellCastEvent>>(&EventDispatcherManager::spellCastEvent, "spellCastEvent");
    //set<EventListenService<TESPlayerBowShotEvent>>(&EventDispatcherManager::unknownDispatcher42, "unknownDispatcher42");
    //set<EventListenService<TESTopicInfoEvent>>(&EventDispatcherManager::topicInfoEvent, "topicInfoEvent");
    //set<EventListenService<TESTrackedStatsEvent>>(&EventDispatcherManager::trackedStatsEvent, "trackedStatsEvent");
    //set<EventListenService<TESTrapHitEvent>>(&EventDispatcherManager::trapHitEvent, "trapHitEvent");
    //set<EventListenService<TESTriggerEvent>>(&EventDispatcherManager::triggerEvent, "triggerEvent");
    //set<EventListenService<TESTriggerEnterEvent>>(&EventDispatcherManager::triggerEnterEvent, "triggerEnterEvent");
    //set<EventListenService<TESTriggerLeaveEvent>>(&EventDispatcherManager::triggerLeaveEvent, "triggerLeaveEvent");
    //set<EventListenService<TESUniqueIDChangeEvent>>(&EventDispatcherManager::uniqueIDChangeEvent, "uniqueIDChangeEvent");
    //set<EventListenService<UnknownEvent>>(&EventDispatcherManager::unknownDispatcher50, "unknownDispatcher50"); // waitevent
    //set<EventListenService<UnknownEvent>>(&EventDispatcherManager::unknownDispatcher51, "unknownDispatcher51"); // TESWaitStopEvent
    //set<EventListenService<TESSwitchRaceCompleteEvent>>(&EventDispatcherManager::switchRaceCompleteEvent, "switchRaceCompleteEvent");
    //set<EventListenService<TESFastTravelEndEvent>>(&EventDispatcherManager::fastTravelEndEvent, "fastTravelEndEvent");

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
