#include <TiltedOnlinePCH.h>
#include <Events/DisconnectedEvent.h>
#include <Events/UpdateEvent.h>
#include <Messages/ServerSkySeed.h>
#include <World.h>

#include <Services/SkyService.h>

#include <Games/Skyrim/Shared/Sky/Sky.h>

#define SKY_DEBUG 0

constexpr float kTransitionSpeed = 5.f;

bool SkyService::s_mOnline = false;

bool SkyService::UseOnlineTick() noexcept
{
    return s_mOnline;
}

SkyService::SkyService(World& aWorld, entt::dispatcher& aDispatcher) : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&SkyService::HandleUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&SkyService::OnDisconnected>(this);
    m_seedConnection = aDispatcher.sink<ServerSkySeed>().connect<&SkyService::OnSeedAssignment>(this);

#if SKY_DEBUG
    m_drawConnection = aImguiService.OnDraw.connect<&SkyService::OnDraw>(this);
#endif
}

void SkyService::OnSeedAssignment(const ServerSkySeed& acMessage) noexcept
{
    // Apply server known weather state.
    m_WeatherSeed = acMessage.Seed;
    s_mOnline = true;

    // manually force update the weather.
    // FindWeatherOfType
    // this immedeate call will hopefully use the custom seed~!
    Sky::GetInstance()->SetWeather(, true, true);
}

void SkyService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    s_mOnline = false;
}

void SkyService::HandleUpdate(const UpdateEvent& aEvent) noexcept
{
}

void SkyService::OnDraw() noexcept
{
}
