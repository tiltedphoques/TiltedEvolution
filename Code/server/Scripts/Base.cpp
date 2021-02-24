#include <stdafx.h>

#include <Scripts/Base.h>

void LuaPrint(sol::this_state aState)
{
    const sol::state_view state(aState);

    std::ostringstream oss;
    oss << "[script:" << state.registry()["PLUGIN_NAME"].get<std::string>() << "] ";

    const auto L = aState.lua_state();

    const auto count = lua_gettop(L);

    lua_getglobal(L, "tostring");

    for (int i = 1; i <= count; i++)
    {
        lua_pushvalue(L, -1);
        lua_pushvalue(L, i);

        lua_call(L, 1, 1);

        size_t l;
        const char* pStr = lua_tolstring(L, -1, &l);

        if (pStr == nullptr)
            return;

        if (i > 1)
            oss << '\t';

        oss << pStr;

        lua_pop(L, 1);
    }

    spdlog::info(oss.str());
}
