set_xmakever("2.5.3")

-- c code will use c99,
-- cxx code will use cxx17 currently, cxx20 soon
set_languages("c99", "cxx17")

-- 64 bit only, as SkyrimLE support is history
set_arch("x64")
set_warnings("all")
add_vectorexts("sse", "sse2", "sse3", "ssse3")

-- build configurations
add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

add_requires("entt")

option("vivox", {default = os.exists("Libraries/Vivox/SDK"), showmenu = true, description = "Enable Vivox"})

if is_mode("debug") then
    add_defines("DEBUG")
end

if is_plat("windows") then
    add_defines("NOMINMAX")
end

-- add projects
includes("Libraries")
includes("Code")
