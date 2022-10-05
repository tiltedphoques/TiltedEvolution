target("LuaScriptingRuntime")
    set_group("ServerPlugins")
    set_kind("shared")
    -- replicate https://github.com/xmake-io/xmake/blob/6cf488d207c3009aba6319759a2cc2fb6aa7a405/core/src/xmake/xmake.lua#L25
    set_targetdir("$(buildir)/$(plat)/$(arch)/$(mode)/plugins")
    add_includedirs(
        ".",
        "../.." -- main dir
    ) 
    add_headerfiles("**.h")
    add_files("**.c", "**.cpp")
    add_deps("SkyrimTogetherServer")