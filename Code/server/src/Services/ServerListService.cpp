#include <Services/ServerListService.h>
#include <Events/UpdateEvent.h>
#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveEvent.h>
#include <GameServer.h>

#include <Components.h>

#include <httplib.h>

ServerListService::ServerListService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld), m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&ServerListService::OnUpdate>(this)),
      m_nextAnnounce(std::chrono::seconds(0))
{
}

void ServerListService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    if (m_nextAnnounce < std::chrono::steady_clock::now())
    {
        Announce();

        m_nextAnnounce = (std::chrono::steady_clock::now() + std::chrono::minutes(1));
    }
}

void ServerListService::OnPlayerJoin(const PlayerJoinEvent& acEvent) noexcept
{
    Announce();

    m_nextAnnounce = (std::chrono::steady_clock::now() + std::chrono::minutes(1));
}

void ServerListService::OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept
{
    Announce();

    m_nextAnnounce = (std::chrono::steady_clock::now() + std::chrono::minutes(1));
}

void ServerListService::Announce() const noexcept
{
    uint32_t playerCount = m_world.view<PlayerComponent>().size();
    uint32_t port = GameServer::Get()->GetPort();
    const auto& cName = GameServer::Get()->GetName();

    auto DoPost = [playerCount, port, cName]() {
        const httplib::Params params{
            {"port", std::to_string(port)}, {"player_count", std::to_string(playerCount)}, {"name", std::string(cName.c_str(), cName.size())}};

        httplib::Client client("https://reborn-list.skyrim-together.com");
        const auto response = client.Post("/announce", params);

        // If we send a 203 it means we banned this server
        if (response && response->status == 203)
        {
            GameServer::Get()->Close();
        }
    };

    std::thread(DoPost).detach();
}
