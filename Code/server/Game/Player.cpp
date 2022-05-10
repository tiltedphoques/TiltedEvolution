#include "Player.h"
#include <GameServer.h>

static uint32_t GenerateId()
{
    static std::atomic<uint32_t> s_counter;
    return s_counter.fetch_add(1);
}

Player::Player(ConnectionId_t aConnectionId)
    : m_id(GenerateId())
    , m_connectionId(aConnectionId)
{
    
}

Player::Player(Player&& aRhs) noexcept
    : m_id{std::exchange(aRhs.m_id, 0)}
    , m_connectionId{std::exchange(aRhs.m_connectionId, 0)}
    , m_character{std::exchange(aRhs.m_character, std::nullopt)}
    , m_mods{std::exchange(aRhs.m_mods, {})}
    , m_modIds{std::exchange(aRhs.m_modIds, {})}
    , m_discordId{std::exchange(aRhs.m_discordId, 0)}
    , m_endpoint{std::exchange(aRhs.m_endpoint, {})}
    , m_username{std::exchange(aRhs.m_username, {})}
    , m_party{std::exchange(aRhs.m_party, {})}
    , m_questLog{std::exchange(aRhs.m_questLog, {})}
    , m_cell{std::exchange(aRhs.m_cell, {})}
{
}

CellIdComponent& Player::GetCellComponent() noexcept
{
    return m_cell;
}

const CellIdComponent& Player::GetCellComponent() const noexcept
{
    return m_cell;
}

QuestLogComponent& Player::GetQuestLogComponent() noexcept
{
    return m_questLog;
}

const QuestLogComponent& Player::GetQuestLogComponent() const noexcept
{
    return m_questLog;
}

void Player::SetDiscordId(uint64_t aDiscordId) noexcept
{
    m_discordId = aDiscordId;
}

void Player::SetEndpoint(String aEndpoint) noexcept
{
    m_endpoint = std::move(aEndpoint);
}

void Player::SetUsername(String aUsername) noexcept
{
    m_username = std::move(aUsername);
}

void Player::SetMods(Vector<String> aMods) noexcept
{
    m_mods = std::move(aMods);
}

void Player::SetModIds(Vector<uint16_t> aModIds) noexcept
{
    m_modIds = std::move(aModIds);
}

void Player::SetCharacter(entt::entity aCharacter) noexcept
{
    m_character = aCharacter;
}

void Player::SetStringCacheId(uint32_t aStringCacheId) noexcept
{
    m_stringCacheId = aStringCacheId;
}

void Player::SetLevel(uint16_t aLevel) noexcept
{
    m_level = aLevel;
}

void Player::SetCellComponent(const CellIdComponent& aCellComponent) noexcept
{
    m_cell = aCellComponent;
}

void Player::Send(const ServerMessage& acServerMessage) const
{
    GameServer::Get()->Send(GetConnectionId(), acServerMessage);
}
