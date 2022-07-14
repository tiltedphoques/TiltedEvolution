#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveEvent.h>
#include <Events/UpdateEvent.h>
#include <GameServer.h>
#include <Services/ServerListService.h>

#include <console/Setting.h>
#include <future>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

static constexpr char kMasterServerEndpoint[] =
#if TP_SKYRIM
    //"http://127.0.0.1:8000";
    "https://skyrim-reborn-list.skyrim-together.com";
#elif TP_FALLOUT
    "https://fallout-reborn-list.skyrim-together.com";
#endif

static constexpr uint16_t kPlayerMaxCap = 1000;

static Console::Setting bAnnounceServer{"LiveServices:bAnnounceServer",
                                        "Whether to list the server on the public server list", false};

ServerListService::ServerListService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld), m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&ServerListService::OnUpdate>(this)),
      m_nextAnnounce(std::chrono::seconds(0))
{
    if (!bAnnounceServer)
        spdlog::warn("bAnnounceServer is set to false. The server will not show up as a public server. "
                     "If you are just playing with friends, this is probably what you want.");

    // TODO: list pw protected servers on server list
    if (GameServer::Get()->IsPasswordProtected())
    {
        spdlog::warn("Your server will not show up on the server list because this server has a password.");
        bAnnounceServer = false;
    }
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
    auto* pServer = GameServer::Get();
    const auto& cInfo = pServer->GetInfo();
    auto pc = static_cast<uint16_t>(m_world.GetPlayerManager().Count());

    auto f = std::async(std::launch::async, PostAnnouncement, cInfo.name, cInfo.desc, cInfo.icon_url,
                        pServer->GetPort(), cInfo.tick_rate, pc, kPlayerMaxCap, cInfo.tagList, bAnnounceServer);
}

void ServerListService::PostAnnouncement(String acName, String acDesc, String acIconUrl, uint16_t aPort, uint16_t aTick,
                                         uint16_t aPlayerCount, uint16_t aPlayerMaxCount, String acTagList, bool aPublic) noexcept
{
    const std::string kVersion{BUILD_COMMIT};
    const httplib::Params params{
        {"name", std::string(acName.c_str(), acName.size())},
        {"desc", std::string(acDesc.c_str(), acDesc.size())},
        {"icon_url", std::string(acIconUrl.c_str(), acIconUrl.size())},
        {"version", std::string(kVersion.c_str(), kVersion.size())},
        {"port", std::to_string(aPort)},
        {"tick", std::to_string(aTick)},
        {"player_count", std::to_string(aPlayerCount)},
        {"max_player_count", std::to_string(aPlayerMaxCount)},
        {"tags", std::string(acTagList.c_str(), acTagList.size())},
        {"public", aPublic ? "true" : "false" },
    };

    httplib::Client client(kMasterServerEndpoint);
    client.enable_server_certificate_verification(false);
    const auto response = client.Post("/announce", params);

    // If we send a 203 it means we banned this server
    if (response)
    {
        if (response->status == 403)
            GameServer::Get()->Kill();
        else if (response->status != 200)
            spdlog::error("Server list error! {}", response->body);

    }
    else
    {
        spdlog::error("Server could not reach the server list! {}", to_string(response.error()));
    }
}
