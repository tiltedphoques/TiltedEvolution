add_requires("sol2", {configs = {lua = "lua"}})
--add_requires("lua")
-- {configs = {shared = false}}
target("LuaScriptingRuntime")
    set_group("ServerPlugins")
    set_kind("shared")
    -- replicate https://github.com/xmake-io/xmake/blob/6cf488d207c3009aba6319759a2cc2fb6aa7a405/core/src/xmake/xmake.lua#L25
    set_targetdir("$(buildir)/$(plat)/$(arch)/$(mode)/plugins")
    add_includedirs(
        ".",
        "../.." -- main dir
    ) 
    set_pcxxheader("Pch.h")
    add_headerfiles("**.h")
    add_files("**.c", "**.cpp")
    add_deps("SkyrimTogetherServer")
    --add_packages("lua",  {links = "lua"})
    add_packages("sol2")