#pragma once

#include "Pch.h"

class LuaStateHolder
{
public:
    LuaStateHolder()
    {
        m_state = luaL_newstate();
#if LUA_VERSION_NUM >= 504
        lua_gc(m_state, LUA_GCGEN, 0, 0); /* GC in generational mode */
#endif
    }

    ~LuaStateHolder() { Close(); }

    void Close()
    {
        if (m_state)
        {
            lua_close(m_state);
            m_state = nullptr;
        }
    }

    operator lua_State*() { return m_state; }

private:
    lua_State* m_state;
};
