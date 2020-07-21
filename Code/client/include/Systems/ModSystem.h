#pragma once

struct Mods;
struct GameId;

struct ModSystem
{
    ModSystem(entt::dispatcher& aDispatcher) noexcept;
    ~ModSystem() = default;

    TP_NOCOPYMOVE(ModSystem);

    bool GetServerModId(uint32_t aGameId, uint32_t& aModId, uint32_t& aBaseId) const noexcept;
    uint32_t GetGameId(uint32_t aServerId, uint32_t aFormId) const noexcept;
    uint32_t GetGameId(const GameId& acGameId) const noexcept;

protected:

    void HandleMods(const Mods& acMods) noexcept;

private:

    struct GameMod
    {
        uint16_t id;
        bool isLite;
    };

    entt::scoped_connection m_modsConnection;
    Map<uint32_t, uint32_t> m_liteToServer;
    Map<uint32_t, GameMod> m_serverToGame;
    uint32_t m_standardToServer[0x100];
};
