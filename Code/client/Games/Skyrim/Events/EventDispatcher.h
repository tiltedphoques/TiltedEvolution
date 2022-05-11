#pragma once

#include <TESObjectREFR.h>

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

struct BGSEventProcessedEvent
{

};

struct TESActivateEvent
{
    TESObjectREFR* object;
};

struct TESActiveEffectApplyRemove
{
    TESObjectREFR* hCaster;
    TESObjectREFR* hTarget;
    uint16_t usActiveEffectUniqueID;
    bool bIsApplied;
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
    uint32_t oldContainerID;
    uint32_t newContainerID;
    uint8_t pad8[0xC];
};

struct TESDeathEvent
{
    TESObjectREFR* pActorDying;
    TESObjectREFR* pActorKiller;
    bool isDead;
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
    TESObjectREFR* hit;
    TESObjectREFR* hitter;
};

struct TESLoadGameEvent
{

};

struct TESLockChangedEvent
{

};

struct TESMagicEffectApplyEvent
{
    TESObjectREFR* hTarget;
    TESObjectREFR* hCaster;
    uint32_t uiMagicEffectFormID;
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

struct TESInitScriptEvent
{

};

struct TESPerkEntryRunEvent
{

};

struct TESQuestInitEvent
{
    uint32_t formId;
};

struct TESQuestStageEvent
{
    void* callback;
    uint32_t formId;
    uint16_t stageId;
    bool bUnk;
};

struct TESQuestStartStopEvent
{
    uint32_t formId;
};

struct TESQuestStageItemDoneEvent
{
    uint32_t formId;
    uint16_t stageId;
    bool unk;
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
    TESObjectREFR* hSpeakerRef;
    void* pCallback;
    uint32_t uiTopicInfoFormID;
    int32_t eType;
    uint16_t usStage;
};

struct TESTrackedStatsEvent
{

};

struct TESTrapHitEvent
{

};

struct TESTriggerEvent
{
    TESObjectREFR* pTrigger;
    TESObjectREFR* pActionRef;
};

struct TESTriggerEnterEvent
{
    TESObjectREFR* pTrigger;
    TESObjectREFR* pActionRef;
};

struct TESTriggerLeaveEvent
{
    TESObjectREFR* pTrigger;
    TESObjectREFR* pActionRef;
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

    EventDispatcher<BGSEventProcessedEvent> eventProcessedEvent;
    EventDispatcher<TESActivateEvent> activateEvent;
    EventDispatcher<TESActiveEffectApplyRemove> activeEffectApplyRemove;
    EventDispatcher<TESActorLocationChangeEvent> actorLocationChangeEvent;
    EventDispatcher<TESBookReadEvent> bookReadEvent;
    EventDispatcher<TESCellAttachDetachEvent> cellAttachDetachEvent;
    EventDispatcher<TESCellFullyLoadedEvent> cellFullyLoadedEvent;
    EventDispatcher<UnknownEvent> unknownDispatcher8; //apply decals event
    EventDispatcher<TESCombatEvent> combatEvent;
    EventDispatcher<TESContainerChangedEvent> containerChangedEvent;
    EventDispatcher<TESDeathEvent> deathEvent;
    EventDispatcher<TESDestructionStageChangedEvent> destructionStageChangedEventDispatcher;
    EventDispatcher<TESEnterBleedoutEvent> enterBleedoutEvent;
    EventDispatcher<TESEquipEvent> equipEvent;
    EventDispatcher<TESFormDeleteEvent> formDeleteEvent;
    EventDispatcher<TESFurnitureEvent> furnitureEvent;
    EventDispatcher<TESGrabReleaseEvent> grabReleaseEvent;
    EventDispatcher<TESHitEvent> hitEvent; //validated
    EventDispatcher<TESInitScriptEvent> initScriptEvent;
    EventDispatcher<TESLoadGameEvent> loadGameEvent;
    EventDispatcher<TESLockChangedEvent> lockChangedEvent;
    EventDispatcher<TESMagicEffectApplyEvent> magicEffectApplyEvent;
    EventDispatcher<TESMagicWardHitEvent> magicWardHitEvent;
    EventDispatcher<TESMoveAttachDetachEvent> moveAttachDetachEvent;
    EventDispatcher<TESObjectLoadedEvent> objectLoadedEvent;
    EventDispatcher<TESObjectREFRTranslationEvent> objectREFRTranslationEvent;
    EventDispatcher<TESOpenCloseEvent> openCloseEvent;
    EventDispatcher<TESPackageEvent> packageEvent;
    EventDispatcher<TESPerkEntryRunEvent> perkEntryRunEvent;
    EventDispatcher<TESQuestInitEvent> questInitEvent; //9F8
    EventDispatcher<TESQuestStageEvent> questStageEvent;
    EventDispatcher<TESQuestStageItemDoneEvent> questStageItemDoneEvent; // AA8, validated StageItemFinishedCallback::TriggerItemDoneEvent
    EventDispatcher<TESQuestStartStopEvent> questStartStopEvent; // TESResolveNPCTemplatesEvent
    EventDispatcher<TESResetEvent> resetEvent; //validated 0xB58
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
    EventDispatcher<UnknownEvent> unknownDispatcher50; //waitevent
    EventDispatcher<UnknownEvent> unknownDispatcher51; //TESWaitStopEvent
    EventDispatcher<TESSwitchRaceCompleteEvent> switchRaceCompleteEvent;
    EventDispatcher<TESFastTravelEndEvent> fastTravelEndEvent;
};

//constexpr auto x = offsetof(EventDispatcherManager, unkx);

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
static_assert(offsetof(EventDispatcherManager, questInitEvent) == 0x9F8);
static_assert(offsetof(EventDispatcherManager, questStageEvent) == 0xA50);
static_assert(offsetof(EventDispatcherManager, questStartStopEvent) == 0xB00);
