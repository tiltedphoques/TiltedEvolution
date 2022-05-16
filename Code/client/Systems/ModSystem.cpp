#include <TiltedOnlinePCH.h>

#include <Systems/ModSystem.h>

#include <Structs/Mods.h>
#include <Structs/GameId.h>

#include <Games/TES.h>

ModSystem::ModSystem(entt::dispatcher& aDispatcher) noexcept
{
    std::memset(m_standardToServer, 0, sizeof(m_standardToServer));
    // Deal with temporary ids
    m_standardToServer[0xFF] = std::numeric_limits<uint32_t>::max();

    m_modsConnection = aDispatcher.sink<Mods>().connect<&ModSystem::HandleMods>(this);
}

bool ModSystem::GetServerModId(const uint32_t aGameId, uint32_t& aModId, uint32_t& aBaseId) const noexcept
{
    uint32_t hiByte = aGameId >> 24;

    // If it's a lite mod
    if (hiByte == 0xFE)
    {
        const uint16_t liteId = ((aGameId & 0x00FFF000) >> 12) & 0xFFFF;

        const auto itor = m_liteToServer.find(liteId);
        if (itor != std::end(m_liteToServer))
        {
            aModId = itor->second;
            aBaseId = liteId;
            return true;
        }

        return false;
    }

    // Here we have a standard mod
    aModId = m_standardToServer[hiByte & 0xFF];
    aBaseId = aGameId & 0x00FFFFFFu;

    return true;
}

bool ModSystem::GetServerModId(uint32_t aGameId, GameId& aServerId) const noexcept
{
    return GetServerModId(aGameId, aServerId.ModId, aServerId.BaseId);
}

uint32_t ModSystem::GetGameId(uint32_t aServerId, uint32_t aFormId) const noexcept
{
    auto itor = m_serverToGame.find(aServerId);
    if (itor != std::end(m_serverToGame))
    {
        if (itor->second.isLite)
        {
            aFormId &= 0xFFFu;
            aFormId |= 0xFE000000u;
            aFormId |= uint32_t(itor->second.id) << 12;
        }
        else
        {
            aFormId &= 0x00FFFFFFu;
            aFormId |= uint32_t(itor->second.id) << 24;
        }

        return aFormId;
    }

    return 0;
}

uint32_t ModSystem::GetGameId(const GameId& acGameId) const noexcept
{
    return GetGameId(acGameId.ModId, acGameId.BaseId);
}

void ModSystem::HandleMods(const Mods& acMods) noexcept
{
    m_serverToGame.clear();
    m_liteToServer.clear();
    std::memset(m_standardToServer, 0, sizeof(m_standardToServer));
    m_standardToServer[0xFF] = std::numeric_limits<uint32_t>::max();

    const auto pModManager = ModManager::Get();

    for (const auto& mod : acMods.ModList)
    {
        if (Mod* pMod = pModManager->GetByName(mod.Filename.c_str()))
        {
            if (mod.IsLite)
            {
                m_serverToGame.emplace(mod.Id, GameMod{static_cast<uint16_t>(pMod->GetId() & 0xFFFu), true});
                m_liteToServer.emplace(pMod->GetId(), mod.Id);
            }
            else
            {
                m_serverToGame.emplace(mod.Id, GameMod{static_cast<uint16_t>(pMod->GetId() & 0xFFu), false});
                m_standardToServer[pMod->GetId() & 0xFF] = mod.Id;
            }
        }
        else
        {
            spdlog::error("Failed to find mod {}, is lite? {}, id: {:X}", mod.Filename.c_str(), mod.IsLite, mod.Id);
        }
    }
}
