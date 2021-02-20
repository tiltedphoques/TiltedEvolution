#include <TiltedOnlinePCH.h>

#include <Services/RunnerService.h>

#include <Events/UpdateEvent.h>

RunnerService::RunnerService(entt::dispatcher& aDispatcher) noexcept
    : m_dispatcher(aDispatcher)
{
}

void RunnerService::Queue(std::function<void()> aFunctor) noexcept
{
    m_runner.Add(std::move(aFunctor));
}

void RunnerService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    m_runner.Drain();
}
