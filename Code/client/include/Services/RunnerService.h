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
    void Trigger(const T& acEvent)
    {
        m_runner.Add([acEvent, this]()
        {
            m_dispatcher.trigger(acEvent);
        });
    }

    void Queue(std::function<void()> aFunctor) noexcept;

private:

    entt::dispatcher& m_dispatcher;
    TiltedPhoques::TaskQueue m_runner;
};

