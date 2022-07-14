#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveEvent.h>
#include <Services/LuaService.h>
#include <encoding/Messages/RequestInventoryChanges.h>
#include <server/Events/PacketEvent.h>

#include <fstream>

namespace
{
struct LuaInventory
{
    static sol::table ToTable(const Inventory::Entry& acEntry, sol::state& aState)
    {
        sol::table result = aState.create_table();
        result["baseId"] = acEntry.BaseId;
        result["count"] = acEntry.Count;
        result["extraCharge"] = acEntry.ExtraCharge;
        result["extraEnchantId"] = acEntry.ExtraEnchantId.ToUint32();
        result["extraEnchantCharge"] = acEntry.ExtraEnchantCharge;
        result["enchantData"] = ToTable(acEntry.EnchantData, aState);
        result["extraHealth"] = acEntry.ExtraHealth;
        result["extraPoisonId"] = acEntry.ExtraPoisonId.ToUint32();
        result["extraPoisonCount"] = acEntry.ExtraPoisonCount;
        result["extraSoulLevel"] = acEntry.ExtraSoulLevel;
        result["extraEnchantRemoveUnequip"] = acEntry.ExtraEnchantRemoveUnequip;
        result["extraWorn"] = acEntry.ExtraWorn;
        result["extraWornLeft"] = acEntry.ExtraWornLeft;
        result["isQuestItem"] = acEntry.IsQuestItem;
        return result;
    }

    static sol::table ToTable(const Inventory::EnchantmentData& acEnchantmentData, sol::state& aState)
    {
        sol::table result = aState.create_table();
        result["isWeapon"] = acEnchantmentData.IsWeapon;
        result["effects"] = ToTable(acEnchantmentData.Effects, aState);
        return result;
    }

    static sol::table ToTable(const Vector<Inventory::EffectItem>& aEffects, sol::state& aState)
    {
        sol::table result = aState.create_table();
        for (auto& item : aEffects)
        {
            result.add(ToTable(item, aState));
        }
        return result;
    }

    static sol::table ToTable(const Inventory::EffectItem& acEffect, sol::state& aState)
    {
        sol::table result;
        result["magnitude"] = acEffect.Magnitude;
        result["area"] = acEffect.Area;
        result["duration"] = acEffect.Duration;
        result["rawCost"] = acEffect.RawCost;
        result["effectId"] = acEffect.EffectId.ToUint32();
        return result;
    }
};
} // namespace

LuaService::LuaService(World& aWorld, entt::dispatcher& aDispatcher) noexcept : m_world(aWorld)
{
    m_playerJoinConnection = aDispatcher.sink<PlayerJoinEvent>().connect<&LuaService::OnPlayerJoin>(this);
    m_playerJoinConnection = aDispatcher.sink<PlayerLeaveEvent>().connect<&LuaService::OnPlayerLeave>(this);
    m_notifyInventoryChangesConnection =
        aDispatcher.sink<PacketEvent<RequestInventoryChanges>>().connect<&LuaService::OnInventoryChanges>(this);

    InitializeCETLikeScripting();
}

void LuaService::OnPlayerJoin(const PlayerJoinEvent& acMessage) noexcept
{
    if (auto function = GetFunction("onPlayerJoin"))
    {
        auto playerId = acMessage.pPlayer->GetId();
        auto cellId = acMessage.CellId.ToUint32();
        auto worldSpaceId = acMessage.WorldSpaceId.ToUint32();

        sol::protected_function f(*function);
        HandleCallResult(f(playerId, cellId, worldSpaceId));
    }
}

void LuaService::OnPlayerLeave(const PlayerLeaveEvent& acMessage) noexcept
{
    if (auto function = GetFunction("onPlayerLeave"))
    {
        auto playerId = acMessage.pPlayer->GetId();

        sol::protected_function f(*function);
        HandleCallResult(f(playerId));
    }
}

void LuaService::OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) noexcept
{
    if (auto function = GetFunction("onInventoryChanges"))
    {
        auto playerId = acMessage.pPlayer->GetId();
        auto serverId = acMessage.Packet.ServerId;
        auto item = LuaInventory::ToTable(acMessage.Packet.Item, m_lua);
        auto drop = acMessage.Packet.Drop;

        sol::protected_function f(*function);
        HandleCallResult(f(playerId, serverId, item, drop));
    }
}

// https://github.com/yamashi/CyberEngineTweaks/blob/master/src/scripting/Scripting.cpp
void LuaService::InitializeCETLikeScripting() noexcept
{
    m_lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::io, sol::lib::math, sol::lib::package,
                         sol::lib::os, sol::lib::table, sol::lib::bit32);

    m_lua["print"] = [](sol::variadic_args aArgs, sol::this_state aState) {
        std::ostringstream oss;
        sol::state_view s(aState);
        for (auto it = aArgs.cbegin(); it != aArgs.cend(); ++it)
        {
            if (it != aArgs.cbegin())
            {
                oss << " ";
            }
            std::string str = s["tostring"]((*it).get<sol::object>());
            oss << str;
        }

        // In CET we do spdlog::get("scripting")->info instead
        spdlog::info(oss.str());
    };

    // execute autoexec.lua inside our default script directory
    if (std::filesystem::exists("scripts/autoexec.lua"))
    {
        m_lua.do_file("scripts/autoexec.lua");
    }
    else
    {
        spdlog::warn("Missing scripts/autoexec.lua!");
    }
}

std::optional<sol::function> LuaService::GetFunction(const char* aEventName) noexcept
{
    std::ifstream f("scripts/autoexec.lua");
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::string fileContents = buffer.str();

    if (fileContents != m_previousFileContents)
    {
        m_lua.do_file("scripts/autoexec.lua");
        m_previousFileContents = fileContents;

        spdlog::info("Hot reloaded scripts/autoexec.lua");
    }
    f.close();

    sol::lua_value value = m_lua.globals()[aEventName];

    if (value.is<sol::nil_t>())
    {
        // Callback wasn't set by a script. It's expected, doing nothing
        return std::nullopt;
    }

    if (!value.is<sol::function>())
    {
        spdlog::warn("Expected {} to be a function, ignoring", aEventName);
        return std::nullopt;
    }

    return value.as<sol::function>();
}

void LuaService::HandleCallResult(const sol::protected_function_result& aResult) noexcept
{
    if (!aResult.valid())
    {
        sol::error err = aResult;
        std::string what = err.what();
        spdlog::error("{}", what);
    }
}
