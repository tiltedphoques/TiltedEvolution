#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveEvent.h>
#include <Services/LuaService.h>
#include <encoding/Messages/RequestInventoryChanges.h>
#include <server/Events/PacketEvent.h>

LuaService::LuaService(World& aWorld, entt::dispatcher& aDispatcher) noexcept : m_world(aWorld)
{
    m_playerJoinConnection = aDispatcher.sink<PlayerJoinEvent>().connect<&LuaService::OnPlayerJoin>(this);
    m_playerJoinConnection = aDispatcher.sink<PlayerLeaveEvent>().connect<&LuaService::OnPlayerLeave>(this);
    m_notifyInventoryChangesConnection =
        aDispatcher.sink<PacketEvent<RequestInventoryChanges>>().connect<&LuaService::OnInventoryChanges>(this);

    InitializeCETLikeScripting();
}

void LuaService::OnPlayerJoin(const PlayerJoinEvent& acMessage) const noexcept
{
    if (auto function = GetFunction("onPlayerJoin"))
    {
        auto playerId = acMessage.pPlayer->GetId();
        auto cellFormId = acMessage.CellId.ToUint32();
        auto worldSpaceFormId = acMessage.WorldSpaceId.ToUint32();

        function->call(playerId, cellFormId, worldSpaceFormId);
    }
}

void LuaService::OnPlayerLeave(const PlayerLeaveEvent& acMessage) const noexcept
{
    if (auto function = GetFunction("onPlayerLeave"))
    {
        auto playerId = acMessage.pPlayer->GetId();

        function->call(playerId);
    }
}

void LuaService::OnInventoryChanges(const PacketEvent<RequestInventoryChanges>& acMessage) const noexcept
{
    if (auto function = GetFunction("onInventoryChanges"))
    {
        auto playerId = acMessage.pPlayer->GetId();

        function->call(playerId);
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

std::optional<sol::function> LuaService::GetFunction(const char* aEventName) const noexcept
{
    sol::lua_value value = m_lua.get(aEventName);

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
