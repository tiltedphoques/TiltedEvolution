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

#define DECLARE_DISPATCHER(name, address) \
inline EventDispatcher<name>* GetEventDispatcher_##name() \
    { \
    using TGetDispatcher = EventDispatcher<name>*(); \
    POINTER_FALLOUT4(TGetDispatcher, s_getEventDispatcher, address - 0x140000000); \
    return s_getEventDispatcher.Get()(); \
}; \

DECLARE_DISPATCHER(TESQuestStartStopEvent, 0x1404438B0);
DECLARE_DISPATCHER(TESQuestStageItemDoneEvent, 0x140443810);
DECLARE_DISPATCHER(TESQuestStageEvent, 0x140443770);
DECLARE_DISPATCHER(TESActivateEvent, 0x140441C90);
