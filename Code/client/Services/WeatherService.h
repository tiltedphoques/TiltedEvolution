#pragma once

struct World;
struct TransportService;

/**
* @brief Responsible for weather changes, which is controlled on a party-per-party basis.
*/
struct WeatherService
{
    WeatherService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher);
    ~WeatherService() noexcept = default;

    TP_NOCOPYMOVE(WeatherService);

protected:
    //void OnHitEvent(const HitEvent& acEvent) const noexcept;

private:
    World& m_world;
    TransportService& m_transport;

    //entt::scoped_connection m_hitConnection;
};

