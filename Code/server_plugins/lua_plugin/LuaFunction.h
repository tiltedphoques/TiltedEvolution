#pragma once

#include <string>
#include <vector>

#include "LuaStateHolder.h"
#include <lua/lauxlib.h>
#include <server/PluginAPI.h>

class LuaStateHolder;

class LuaFunction
{
  public:
    explicit LuaFunction(LuaStateHolder&, const std::string& aName, const ArgType* apArgs, size_t aCount);

    void ValidateArg(ArgType a, int i);
    void PushArg(ArgType, ScriptFunctionContext&);

    void Invoke(ScriptFunctionContext&);

    auto& GetName() const
    {
        return m_name;
    }

  private:
    std::string m_name;
    std::vector<ArgType> m_args;
    LuaStateHolder& m_State;
};
