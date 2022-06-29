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

// Sadly fallout4 doesn't have a clean manager like skyrim

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

struct TESQuestStageEvent
{
    void* callback;
    uint32_t formId;
    uint16_t stageId;
    bool bUnk;
};

struct TESActivateEvent
{
    TESObjectREFR* object;
};

struct TESLoadGameEvent
{
};

struct BGSInventoryListEvent
{
    enum Type : int16_t
    {
        AddStack = 0x0,
        ChangedStack = 0x1,
        AddNewItem = 0x2,
        RemoveItem = 0x3,
        Clear = 0x4,
        UpdateWeight = 0x5,
    };

    struct Event
    {
        Type ChangeType;
        BSPointerHandle<TESObjectREFR> hOwner;
        TESBoundObject* pObjAffected;
        uint32_t uiCount;
        uint32_t uiStackId;
    };
};

// TODO: idk why, but it can't find POINTER_FALLOUT4
#define POINTER_FALLOUT4(className, variableName, ...) static VersionDbPtr<className> variableName(__VA_ARGS__)

#define DECLARE_DISPATCHER(name, id) \
inline EventDispatcher<name>* GetEventDispatcher_##name() \
    { \
    using TGetDispatcher = EventDispatcher<name>*(); \
    POINTER_FALLOUT4(TGetDispatcher, s_getEventDispatcher, id); \
    return s_getEventDispatcher.Get()(); \
}; \

DECLARE_DISPATCHER(TESQuestStartStopEvent, 1404316);
DECLARE_DISPATCHER(TESQuestStageItemDoneEvent, 181652);
DECLARE_DISPATCHER(TESQuestStageEvent, 540906);
DECLARE_DISPATCHER(TESActivateEvent, 166231);
DECLARE_DISPATCHER(TESLoadGameEvent, 823571);
