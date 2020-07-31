#pragma once

#if TP_SKYRIM

template <class T> struct BSTEventSink;

// Very nasty work around to avoid template code duplication
namespace details
{
    void InternalRegisterSink(void* apEventDispatcher, void* apSink) noexcept;
    void InternalUnRegisterSink(void* apEventDispatcher, void* apSink) noexcept;
    void InternalPushEvent(void* apEventDispatcher, void* apEvent) noexcept;
}

template<class T>
struct EventDispatcher
{
    void RegisterSink(BSTEventSink<T>* apSink) noexcept
    {
        details::InternalRegisterSink(reinterpret_cast<void*>(this), reinterpret_cast<void*>(apSink));
    }

    void UnRegisterSink(BSTEventSink<T>* apSink) noexcept
    {
        details::InternalUnRegisterSink(reinterpret_cast<void*>(this), reinterpret_cast<void*>(apSink));
    }

    void PushEvent(const T* apEvent) noexcept
    {
        details::InternalPushEvent(reinterpret_cast<void*>(this), reinterpret_cast<void*>(apEvent));
    }

    uint8_t pad0[0x58];
};

struct UnknownEvent
{

};

struct TESActivateEvent
{

};

struct TESActiveEffectApplyRemove
{

};

struct TESActorLocationChangeEvent
{

};

struct TESBookReadEvent
{

};

struct TESCellAttachDetachEvent
{

};

struct TESCellFullyLoadedEvent
{

};

struct TESCombatEvent
{

};

struct TESContainerChangedEvent
{

};

struct TESDeathEvent
{

};

struct TESDestructionStageChangedEvent
{

};

struct TESEnterBleedoutEvent
{

};

struct TESEquipEvent
{

};

struct TESFormDeleteEvent
{

};

struct TESFurnitureEvent
{

};

struct TESGrabReleaseEvent
{

};

struct TESHitEvent
{

};

struct TESLoadGameEvent
{

};

struct TESLockChangedEvent
{

};

struct TESMagicEffectApplyEvent
{

};

struct TESMagicWardHitEvent
{

};

struct TESMoveAttachDetachEvent
{

};

struct TESObjectLoadedEvent
{

};

struct TESObjectREFRTranslationEvent
{

};

struct TESOpenCloseEvent
{

};

struct TESPackageEvent
{

};

struct TESPerkEntryRunEvent
{

};

struct TESQuestInitEvent
{

};

struct TESQuestStageEvent
{

};

struct TESResetEvent
{

};

struct TESResolveNPCTemplatesEvent
{

};

struct TESSceneEvent
{

};

struct TESSceneActionEvent
{

};

struct TESScenePhaseEvent
{

};

struct TESSellEvent
{

};

struct TESSleepStartEvent
{

};

struct TESSleepStopEvent
{

};

struct TESSpellCastEvent
{

};

struct TESPlayerBowShotEvent
{

};

struct TESTopicInfoEvent
{

};

struct TESTrackedStatsEvent
{

};

struct TESTrapHitEvent
{

};

struct TESTriggerEvent
{

};

struct TESTriggerEnterEvent
{

};

struct TESTriggerLeaveEvent
{

};

struct TESUniqueIDChangeEvent
{

};

struct TESSwitchRaceCompleteEvent
{

};

struct TESFastTravelEndEvent
{

};

struct EventDispatcherManager
{
    static EventDispatcherManager* Get() noexcept;

    EventDispatcher<UnknownEvent> unknownDispatcher1;
    EventDispatcher<TESActivateEvent> activateEvent;
    EventDispatcher<TESActiveEffectApplyRemove> activeEffectApplyRemove;
    EventDispatcher<TESActorLocationChangeEvent> actorLocationChangeEvent;
    EventDispatcher<TESBookReadEvent> bookReadEvent;
    EventDispatcher<TESCellAttachDetachEvent> cellAttachDetachEvent;
    EventDispatcher<TESCellFullyLoadedEvent> cellFullyLoadedEvent;
    EventDispatcher<UnknownEvent> unknownDispatcher8;
    EventDispatcher<TESCombatEvent> combatEvent;
    EventDispatcher<TESContainerChangedEvent> containerChangedEvent;
    EventDispatcher<TESDeathEvent> deathEvent;
    EventDispatcher<TESDestructionStageChangedEvent> destructionStageChangedEventDispatcher;
    EventDispatcher<TESEnterBleedoutEvent> enterBleedoutEvent;
    EventDispatcher<TESEquipEvent> equipEvent;
    EventDispatcher<TESFormDeleteEvent> formDeleteEvent;
    EventDispatcher<TESFurnitureEvent> furnitureEvent;
    EventDispatcher<TESGrabReleaseEvent> grabReleaseEvent;
    EventDispatcher<TESHitEvent> hitEvent;
    EventDispatcher<TESLoadGameEvent> loadGameEvent;
    EventDispatcher<UnknownEvent> unknownDispatcher19;
    EventDispatcher<TESLockChangedEvent> lockChangedEvent;
    EventDispatcher<TESMagicEffectApplyEvent> magicEffectApplyEvent;
    EventDispatcher<TESMagicWardHitEvent> magicWardHitEvent;
    EventDispatcher<TESMoveAttachDetachEvent> moveAttachDetachEvent;
    EventDispatcher<TESObjectLoadedEvent> objectLoadedEvent;
    EventDispatcher<TESObjectREFRTranslationEvent> objectREFRTranslationEvent;
    EventDispatcher<TESOpenCloseEvent> openCloseEvent;
    EventDispatcher<TESPackageEvent> packageEvent;
    EventDispatcher<TESPerkEntryRunEvent> perkEntryRunEvent;
    EventDispatcher<TESQuestInitEvent> questInitEvent;
    EventDispatcher<TESQuestStageEvent> questStageEvent;
    EventDispatcher<UnknownEvent> unknownDispatcher31;
    EventDispatcher<UnknownEvent> unknownDispatcher32;
    EventDispatcher<TESResetEvent> resetEvent;
    EventDispatcher<TESResolveNPCTemplatesEvent> resolveNPCTemplatesEvent;
    EventDispatcher<TESSceneEvent> sceneEvent;
    EventDispatcher<TESSceneActionEvent> sceneActionEvent;
    EventDispatcher<TESScenePhaseEvent> scenePhaseEvent;
    EventDispatcher<TESSellEvent> sellEvent;
    EventDispatcher<TESSleepStartEvent> unknownDispatcher39;
    EventDispatcher<TESSleepStopEvent> sleepStopEvent;
    EventDispatcher<TESSpellCastEvent> spellCastEvent;
    EventDispatcher<TESPlayerBowShotEvent> unknownDispatcher42;
    EventDispatcher<TESTopicInfoEvent> topicInfoEvent;
    EventDispatcher<TESTrackedStatsEvent> trackedStatsEvent;
    EventDispatcher<TESTrapHitEvent> trapHitEvent;
    EventDispatcher<TESTriggerEvent> triggerEvent;
    EventDispatcher<TESTriggerEnterEvent> triggerEnterEvent;
    EventDispatcher<TESTriggerLeaveEvent> triggerLeaveEvent;
    EventDispatcher<TESUniqueIDChangeEvent> uniqueIDChangeEvent;
    EventDispatcher<UnknownEvent> unknownDispatcher50;
    EventDispatcher<UnknownEvent> unknownDispatcher51;
    EventDispatcher<TESSwitchRaceCompleteEvent> switchRaceCompleteEvent;
    EventDispatcher<TESFastTravelEndEvent> fastTravelEndEvent;
};

static_assert(sizeof(EventDispatcherManager) == 4752);
static_assert(offsetof(EventDispatcherManager, activateEvent) == 88);
static_assert(offsetof(EventDispatcherManager, deathEvent) == 880);
static_assert(offsetof(EventDispatcherManager, lockChangedEvent) == 1760);
static_assert(offsetof(EventDispatcherManager, furnitureEvent) == 1320);
static_assert(offsetof(EventDispatcherManager, resetEvent) == 2904);
static_assert(offsetof(EventDispatcherManager, trackedStatsEvent) == 3872);
static_assert(offsetof(EventDispatcherManager, triggerEvent) == 4048);
static_assert(offsetof(EventDispatcherManager, switchRaceCompleteEvent) == 4576);
static_assert(offsetof(EventDispatcherManager, fastTravelEndEvent) == 4664);

#endif
