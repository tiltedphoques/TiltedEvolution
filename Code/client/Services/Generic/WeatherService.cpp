#include <Services/WeatherService.h>

#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/PartyJoinedEvent.h>
#include <Events/PartyLeftEvent.h>

#include <Sky/Sky.h>
#include <Forms/TESWeather.h>

WeatherService::WeatherService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_transport(aTransport)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&WeatherService::OnUpdate>(this);
    m_disconnectConnection = aDispatcher.sink<DisconnectedEvent>().connect<&WeatherService::OnDisconnected>(this);
    m_partyJoinedConnection = aDispatcher.sink<PartyJoinedEvent>().connect<&WeatherService::OnPartyJoinedEvent>(this);
    m_partyLeftConnection = aDispatcher.sink<PartyLeftEvent>().connect<&WeatherService::OnPartyLeftEvent>(this);
}

void WeatherService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    RunWeatherUpdates(acEvent.Delta);
}

void WeatherService::OnDisconnected(const DisconnectedEvent& acEvent) noexcept
{
    RegainControlOfWeather();
}

void WeatherService::OnPartyJoinedEvent(const PartyJoinedEvent& acEvent) noexcept
{
    Sky::s_shouldUpdateWeather = acEvent.IsLeader;
}

void WeatherService::OnPartyLeftEvent(const PartyLeftEvent& acEvent) noexcept
{
    RegainControlOfWeather();
}

void WeatherService::RunWeatherUpdates(const double acDelta) noexcept
{
    Sky* pSky = Sky::Get();
    if (!pSky)
        return;

    TESWeather* pWeather = pSky->GetWeather();
    if (!pWeather)
    {
        m_cachedWeatherId = 0;
        return;
    }

    // Have to manually check each frame because there's no singular SetWeather being used in-game.
    if (pWeather->formID == m_cachedWeatherId)
        return;

    m_cachedWeatherId = pWeather->formID;

    // TODO: send out new weather
}

void WeatherService::RegainControlOfWeather() noexcept
{
    Sky::s_shouldUpdateWeather = true;
}
