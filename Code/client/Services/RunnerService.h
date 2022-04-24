#pragma once

#include <TiltedCore/TaskQueue.hpp>

struct UpdateEvent;

/**
* @brief Dispatches events.
*/
struct RunnerService
{
    RunnerService(entt::dispatcher& aDispatcher) noexcept;
    ~RunnerService() noexcept = default;

    TP_NOCOPYMOVE(RunnerService);

    /**
    * @brief Executes all queued events every frame.
    */
    void OnUpdate(const UpdateEvent& acUpdateEvent) noexcept;

    /**
    * @brief Queues an event for OnUpdate() to execute.
    */
    template<class T>
    void Trigger(T acEvent)
    {
        m_runner.Add([event = std::move(acEvent), this]()
        {
            m_dispatcher.trigger(std::move(event));
        });
    }

    /**
    * @brief Queues a lambda for OnUpdate() to execute.
    */
    void Queue(std::function<void()> aFunctor) noexcept;

private:

    entt::dispatcher& m_dispatcher;
    TiltedPhoques::TaskQueue m_runner;
};

