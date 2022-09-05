#include <Services/WeatherService.h>

#include <Events/DisconnectedEvent.h>
#include <Events/PartyJoinedEvent.h>
#include <Events/PartyLeftEvent.h>

#include <Sky/Sky.h>

WeatherService::WeatherService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_transport(aTransport)
{
    m_disconnectConnection = aDispatcher.sink<DisconnectedEvent>().connect<&WeatherService::OnDisconnected>(this);
    m_partyJoinedConnection = aDispatcher.sink<PartyJoinedEvent>().connect<&WeatherService::OnPartyJoinedEvent>(this);
    m_partyLeftConnection = aDispatcher.sink<PartyLeftEvent>().connect<&WeatherService::OnPartyLeftEvent>(this);
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

void WeatherService::RegainControlOfWeather() noexcept
{
    Sky::s_shouldUpdateWeather = true;
}
