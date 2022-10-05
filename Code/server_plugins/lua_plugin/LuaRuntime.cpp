
#include <lua/lua.h>
#include <lua/lualib.h>

#include "LuaRuntime.h"

static_assert(sizeof(LUA_INTEGER) == 8, "Lua must be compiled with 64 bit support");

namespace
{
// luaL_openlibs version without io/os libs
constexpr const luaL_Reg kSafeLuaLibraries[] = {{"_G", luaopen_base},
                                                {LUA_TABLIBNAME, luaopen_table},
                                                {LUA_STRLIBNAME, luaopen_string},
                                                {LUA_MATHLIBNAME, luaopen_math},
                                                {LUA_DBLIBNAME, luaopen_debug},
                                                {LUA_COLIBNAME, luaopen_coroutine},
                                                {LUA_UTF8LIBNAME, luaopen_utf8},
                                                {nullptr, nullptr}};

void RegisterSafeLibraries(lua_State* apState)
{
    // safe_openlibs
    const luaL_Reg* lib = kSafeLuaLibraries;
    for (; lib->func; lib++)
    {
        luaL_requiref(apState, lib->name, lib->func, 1);
        lua_pop(apState, 1);
    }
}
} // namespace

LuaRuntime::~LuaRuntime()
{
}

bool LuaRuntime::Initialize()
{
    RegisterSafeLibraries(m_State);

    ArgType args[] = {ArgType::kBool, ArgType::kF32};
    BindScriptFunction("testFunc", nullptr, args, _countof(args));

    ScriptFunctionContext context(2);
    context.Push(true);
    context.Push(32.f);
    CallScriptFunction("testFunc", context);

    WriteLog(LogLevel::kInfo, "Initialized lua runtime\n");
    return true;
}

void LuaRuntime::Shutdown()
{
    WriteLog(LogLevel::kInfo, "Shutting down lua\n");
}

void LuaRuntime::CallScriptFunction(const PluginStringView aName, ScriptFunctionContext& aContext)
{
    auto it = std::find_if(functions_.begin(), functions_.end(),
                           [&](const LuaFunction& registrar) { return registrar.GetName() == aName.data(); });
    if (it == functions_.end())
        return;

    it->Invoke(aContext);
}

void LuaRuntime::BindScriptFunction(const PluginStringView aName, void* apFunctor, const ArgType* apArgs,
                                    const size_t aArgCount)
{
    functions_.emplace_back(m_State, aName.data(), apArgs, aArgCount);

    // https://github.com/citizenfx/fivem/blob/e46db5133c30577f75e985a36f902a626013ac3c/code/components/citizen-scripting-lua/src/LuaScriptRuntime.cpp#L545

    static auto k = [](lua_State* state) -> int {
        __debugbreak();
        return 0;
    };

    // returns nothing
    lua_register(m_State, aName.data(), k);
}
