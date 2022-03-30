set_xmakever("2.6.2")

-- c code will use c99,
set_languages("c99", "cxx20")

set_arch("x64")
set_warnings("all")
add_vectorexts("sse", "sse2", "sse3", "ssse3")

-- build configurations
add_rules("mode.debug", "mode.releasedbg", "mode.release")

if has_config("unitybuild") then
    add_rules("c.unity_build")
    add_rules("c++.unity_build", {batchsize = 12})
end

add_requires("entt", "recastnavigation")

if is_plat("windows") then
    add_cxflags("/bigobj")
end

before_build(function (target)
    import("modules.version")
    local branch, commitHash = version()
    bool_to_number={ [true]=1, [false]=0 }
    local contents = string.format([[
    #pragma once
    #define IS_TESTING %d
    #define IS_SHIPPING %d
    #define IS_DEV %d
    ]], 
    bool_to_number[branch == "testing"], 
    bool_to_number[branch == "master"], 
    bool_to_number[branch ~= "testing" and branch ~= "master"])
    io.writefile("build/BranchInfo.h", contents)
end)

if is_mode("debug") then
    add_cxflags("/FS")
    add_defines("DEBUG")
end

if is_mode("release") then
    add_ldflags("/LTCG", "/OPT:REF")
    add_cxflags("/Ot", "/GL", "/Ob2", "/Oi", "/GS-")
    add_defines("NDEBUG")
    set_optimize("fastest")
end

if is_plat("windows") then
    add_defines("NOMINMAX")
end

-- add projects
includes("Libraries")
includes("Code")
