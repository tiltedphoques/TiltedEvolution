#include <Services/WeatherService.h>

WeatherService::WeatherService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_transport(aTransport)
{
}
