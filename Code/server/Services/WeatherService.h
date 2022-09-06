#pragma once

#include <Events/PacketEvent.h>

struct World;
struct RequestWeatherChange;

struct WeatherService
{
    WeatherService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~WeatherService() noexcept = default;

    TP_NOCOPYMOVE(WeatherService);

protected:
    void OnWeatherChange(const PacketEvent<RequestWeatherChange>& acMessage) const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_weatherChangeConnection;
};
