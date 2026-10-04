#pragma once

#include <TESObjectREFR.h>

template <class T> struct BSTEventSink;

// Very nasty work around to avoid template code duplication
namespace details
{
void InternalRegisterSink(void* apEventDispatcher, void* apSink) noexcept;
void InternalUnRegisterSink(void* apEventDispatcher, void* apSink) noexcept;
void InternalPushEvent(void* apEventDispatcher, void* apEvent) noexcept;
} // namespace details

template <class T> struct EventDispatcher
{
    void RegisterSink(BSTEventSink<T>* apSink) noexcept { details::InternalRegisterSink(reinterpret_cast<void*>(this), reinterpret_cast<void*>(apSink)); }

    void UnRegisterSink(BSTEventSink<T>* apSink) noexcept { details::InternalUnRegisterSink(reinterpret_cast<void*>(this), reinterpret_cast<void*>(apSink)); }

    void PushEvent(const T* apEvent) noexcept { details::InternalPushEvent(reinterpret_cast<void*>(this), reinterpret_cast<void*>(apEvent)); }

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
    uint8_t itemId;
};

struct TESQuestStartStopEvent
{
    uint32_t formId;
    bool started;
    bool failed;
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

// Fallout 4 keeps one static BSTEventSource per event type. TESObjectREFR_Events
// exposes RegisterFor*/UnregisterFor* helpers that take only the sink.
template <class T> struct GlobalEventSource
{
    using TSinkHelper = void(BSTEventSink<T>*);

    uint32_t RegisterId;
    uint32_t UnregisterId;

    void RegisterSink(BSTEventSink<T>* apSink) const noexcept { Call(RegisterId, apSink); }
    void UnRegisterSink(BSTEventSink<T>* apSink) const noexcept { Call(UnregisterId, apSink); }

private:
    static void Call(uint32_t aId, BSTEventSink<T>* apSink) noexcept
    {
        if (auto* pHelper = static_cast<TSinkHelper*>(VersionDb::Get().FindAddressById(aId)))
            pHelper(apSink);
    }
};

struct EventDispatcherManager
{
    static EventDispatcherManager* Get() noexcept
    {
        static EventDispatcherManager s_manager;
        return &s_manager;
    }

    GlobalEventSource<TESActivateEvent> activateEvent{2201553, 2201554};
    GlobalEventSource<TESLoadGameEvent> loadGameEvent{2201657, 2201658};
    GlobalEventSource<TESQuestStageEvent> questStageEvent{2201684, 2201685};
    GlobalEventSource<TESQuestStartStopEvent> questStartStopEvent{2201690, 2201691};
};
