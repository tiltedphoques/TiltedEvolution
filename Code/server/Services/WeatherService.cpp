#include <Services/WeatherService.h>

#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Messages/RequestWeatherChange.h>
#include <Messages/NotifyWeatherChange.h>

WeatherService::WeatherService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_weatherChangeConnection = aDispatcher.sink<PacketEvent<RequestWeatherChange>>().connect<&WeatherService::OnWeatherChange>(this);
}

void WeatherService::OnWeatherChange(const PacketEvent<RequestWeatherChange>& acMessage) const noexcept
{
    NotifyWeatherChange notify{};
    notify.Id = acMessage.Packet.Id;

    if (!acMessage.pPlayer->GetCharacter())
        return;

    const auto origin = *acMessage.pPlayer->GetCharacter();
    GameServer::Get()->SendToPartyInRange(notify, acMessage.pPlayer->GetParty(), origin, acMessage.pPlayer);
}
