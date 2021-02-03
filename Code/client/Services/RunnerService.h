#pragma once

#include <TaskQueue.hpp>

struct UpdateEvent;

struct RunnerService
{
    RunnerService(entt::dispatcher& aDispatcher) noexcept;
    ~RunnerService() noexcept = default;

    TP_NOCOPYMOVE(RunnerService);

    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;

    template<class T>
    void Trigger(T acEvent)
    {
        m_runner.Add([event = std::move(acEvent), this]()
        {
            m_dispatcher.trigger(std::move(event));
        });
    }

    void Queue(std::function<void()> aFunctor) noexcept;

private:

    entt::dispatcher& m_dispatcher;
    TiltedPhoques::TaskQueue m_runner;
};

