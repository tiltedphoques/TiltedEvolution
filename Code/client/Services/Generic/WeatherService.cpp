#include <Services/WeatherService.h>

#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/PartyJoinedEvent.h>
#include <Events/PartyLeftEvent.h>

#include <Messages/RequestWeatherChange.h>
#include <Messages/NotifyWeatherChange.h>

#include <Sky/Sky.h>
#include <Forms/TESWeather.h>

WeatherService::WeatherService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_transport(aTransport)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&WeatherService::OnUpdate>(this);
    m_disconnectConnection = aDispatcher.sink<DisconnectedEvent>().connect<&WeatherService::OnDisconnected>(this);
    m_partyJoinedConnection = aDispatcher.sink<PartyJoinedEvent>().connect<&WeatherService::OnPartyJoinedEvent>(this);
    m_partyLeftConnection = aDispatcher.sink<PartyLeftEvent>().connect<&WeatherService::OnPartyLeftEvent>(this);
    m_weatherChangeConnection = aDispatcher.sink<NotifyWeatherChange>().connect<&WeatherService::OnWeatherChange>(this);
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

void WeatherService::OnWeatherChange(const NotifyWeatherChange& acMessage) noexcept
{
    auto& modSystem = m_world.GetModSystem();
    const uint32_t weatherId = modSystem.GetGameId(acMessage.Id);
    TESWeather* pWeather = Cast<TESWeather>(TESForm::GetById(weatherId));

    if (!pWeather)
    {
        spdlog::error(__FUNCTION__ ": weather not found, form id: {:X}", acMessage.Id.ModId + acMessage.Id.BaseId);
        return;
    }

    Sky::Get()->SetWeather(pWeather);
}

void WeatherService::RunWeatherUpdates(const double acDelta) noexcept
{
    if (!Sky::s_shouldUpdateWeather)
        return;

    Sky* pSky = Sky::Get();
    if (!pSky)
        return;

    TESWeather* pWeather = pSky->GetWeather();
    if (!pWeather)
    {
        m_cachedWeatherId = 0;
        return;
    }

    // This is the map weather, should not be synced.
    if (pWeather->formID == 0xA6858)
        return;

    // Have to manually check each frame because there's no singular SetWeather being used in-game.
    if (pWeather->formID == m_cachedWeatherId)
        return;

    m_cachedWeatherId = pWeather->formID;

    RequestWeatherChange request{};

    auto& modSystem = m_world.GetModSystem();
    if (!modSystem.GetServerModId(pWeather->formID, request.Id))
    {
        spdlog::error(__FUNCTION__ ": weather server ID not found, form id: {:X}", pWeather->formID);
        return;
    }

    m_transport.Send(request);
}

void WeatherService::RegainControlOfWeather() noexcept
{
    Sky::s_shouldUpdateWeather = true;
    Sky::Get()->ResetWeather();
}
