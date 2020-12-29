#pragma once
#include <string>

#include <Games/Events.h>

struct World;
struct ImguiService;
struct UpdateEvent;

struct TransportService;

struct Actor;

template <typename EventT> struct EventListenService : BSTEventSink<EventT>
{
    EventListenService(EventDispatcher<EventT> EventDispatcherManager::*dispatcher, const std::string& message) noexcept
        : m_message(message)
    {
        (EventDispatcherManager::Get()->*dispatcher).RegisterSink(this);
    }

    ~EventListenService() noexcept
    {
    }

    TP_NOCOPYMOVE(EventListenService);

  private:
    std::string m_message;
    BSTEventResult OnEvent(const EventT* aEvent, const EventDispatcher<EventT>* aDispatcher) override
    {
        spdlog::log(spdlog::level::info, std::string("event: ") + m_message);
        return BSTEventResult::kOk;
    }
};
