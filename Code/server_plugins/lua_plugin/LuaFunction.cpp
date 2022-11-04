
#include "LuaFunction.h"
#include <assert.h>

LuaFunction::LuaFunction(LuaStateHolder& aHolder, const std::string& aName, const ArgType* apArgs, size_t aCount)
    : m_State(aHolder), m_name(aName)
{
    m_args.reserve(aCount);
    for (size_t i = 0; i < aCount; i++)
        m_args.push_back(apArgs[i]);

    assert(m_args.size() == aCount);
}

void LuaFunction::ValidateArg(ArgType a, int i)
{
    switch (a)
    {
    case ArgType::kBool:
        luaL_checktype(m_State, i, LUA_TBOOLEAN);
        break;
    // signed integers
    case ArgType::kI8:
    case ArgType::kI16:
    case ArgType::KI32:
    case ArgType::kI64:
        luaL_checkinteger(m_State, i);
        break;
    case ArgType::kU8:
    case ArgType::kU16:
    case ArgType::kU32:
    case ArgType::kU64:
        ((lua_Unsigned)luaL_checkinteger(m_State, i));
        break;
    case ArgType::kF32:
        luaL_checknumber(m_State, i);
        break;
    }
}

void LuaFunction::PushArg(ArgType type, ScriptFunctionContext& aContext)
{
    switch (type)
    {
    case ArgType::kBool:
        lua_pushboolean(m_State, static_cast<int>(aContext.PopBool()));
        break;
    case ArgType::kF32:
        lua_pushnumber(m_State, static_cast<lua_Number>(aContext.PopF32()));
        break;
    case ArgType::kI8:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopI8()));
        break;
    case ArgType::kI16:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopI16()));
        break;
    case ArgType::KI32:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopI32()));
        break;
    case ArgType::kI64:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopI64()));
        break;
    case ArgType::kU8:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopU8()));
        break;
    case ArgType::kU16:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopU16()));
        break;
    case ArgType::kU32:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopU32()));
        break;
    case ArgType::kU64:
        lua_pushinteger(m_State, static_cast<lua_Integer>(aContext.PopU64()));
        break;
    default:
        //__debugbreak();
        break;
    }
}

void LuaFunction::Invoke(ScriptFunctionContext& aContext)
{
    // load on stack
    if (lua_getglobal(m_State, this->m_name.c_str()) != LUA_TFUNCTION)
    {
        WriteLog(LogLevel::kInfo, "Failed to find function %s", this->m_name.c_str());
        return;
    }

    // store the max arg count (it changes when pushing/popping args so we must take a snapshot here)
    const uint32_t argCount{aContext.count()};
    for (uint32_t i = 0; i < argCount; i++)
        PushArg(aContext.GetArg(i), aContext);

    // validate that the args we pushed are actually useful?
    // for (int i = 0; i < m_args.size(); i++)
    //    ValidateArg(m_args[i], i);

    auto result = lua_pcall(m_State, argCount, 1, 0);
    //__debugbreak();
}
