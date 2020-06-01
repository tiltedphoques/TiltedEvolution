#include <Systems/ModSystem.h>

#include <Games/TES.h>

ModSystem::ModSystem(entt::dispatcher& aDispatcher) noexcept
{
    std::memset(m_standardToServer, 0, sizeof(m_standardToServer));
    // Deal with temporary ids
    m_standardToServer[0xFF] = std::numeric_limits<uint32_t>::max();

    m_modsConnection = aDispatcher.sink<TiltedMessages::Mods>().connect<&ModSystem::HandleMods>(this);
}

bool ModSystem::GetServerModId(const uint32_t aGameId, uint32_t& aModId, uint32_t& aBaseId) const noexcept
{
    uint32_t hiByte = aGameId >> 24;

    // If it's a lite mod
    if (hiByte == 0xFE)
    {
        const uint32_t liteId = (aGameId & 0x00FFF000) >> 12;

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

void ModSystem::HandleMods(const TiltedMessages::Mods& acMods) noexcept
{
    m_serverToGame.clear();
    m_liteToServer.clear();
    std::memset(m_standardToServer, 0, sizeof(m_standardToServer));
    m_standardToServer[0xFF] = std::numeric_limits<uint32_t>::max();

    const auto pModManager = ModManager::Get();

    for (auto& mod : acMods.lite_mods())
    {
        const auto pMod = pModManager->GetByName(mod.filename().c_str());
        if (!pMod)
        {
            // Shouldn't happen
            continue;
        }

        m_serverToGame.emplace(mod.id(), GameMod{ pMod->GetId() & 0xFFF, true });
        m_liteToServer.emplace(pMod->GetId(), mod.id());
    }

    for (auto& mod : acMods.standard_mods())
    {
        auto pMod = pModManager->GetByName(mod.filename().c_str());
        if (!pMod)
        {
            // Shouldn't happen
            continue;
        }

        m_serverToGame.emplace(mod.id(), GameMod{ pMod->GetId() & 0xFF, false });
        m_standardToServer[pMod->GetId() & 0xFF] = mod.id();
    }
}
