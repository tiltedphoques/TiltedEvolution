
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

void LuaFunction::Invoke(ScriptFunctionContext& aContext)
{
    // load on stack
    if (lua_getglobal(m_State, this->m_name.c_str()) != LUA_TFUNCTION)
    {
        // TBD handle error
        return;
    }

    // push args
    for (uint32_t i = 0; i < aContext.count(); i++)
    {
        const auto x = aContext.argAt(i);
        __debugbreak();

        switch (x)
        {
        case ArgType::kBool:
            __debugbreak();
            lua_pushboolean(m_State, static_cast<int>(aContext.PopBool()));
            break;
        case ArgType::kF32:
            __debugbreak();
            lua_pushnumber(m_State, static_cast<lua_Number>(aContext.PopF32()));
            break;
        default:
            __debugbreak();
            break;
        }
    }
    // validate that the args we pushed are actually useful?
   // for (int i = 0; i < m_args.size(); i++)
   //     ValidateArg(m_args[i], i);

    bool success = lua_pcall(m_State, m_args.size(), 1, 0) == LUA_OK;
    __debugbreak();
}
