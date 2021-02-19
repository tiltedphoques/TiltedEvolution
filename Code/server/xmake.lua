
local function build_server(name, def)
target(name)
    set_kind("binary")
    set_group("Server")
    --set_basename(name .. "RebornServer")
    set_languages("cxx17")
    add_defines(def)
    add_includedirs(
        ".",
        "../../Libraries/",
        "../../Libraries/entt",
        "../../Libraries/cpp-httplib",
        "../encoding/")
    set_pcxxheader("stdafx.h")
    add_cxxflags("-include $(projectdir)\\code\\server\\stdafx.h", { force=true })
    add_headerfiles(
        "**.h",
        "../encoding/**.h")
    add_files(
        "**.cpp",
        "../encoding/**.cpp")
        
    if name == "SkyrimTogetherServer" then
        del_files("../encoding/Structs/Fallout4/**.cpp")
    end
    if name == "FalloutTogetherServer" then
        del_files("../encoding/Structs/Skyrim/**.cpp")
    end

    add_deps(
        "Common",
        "lua",
        "sqlite3",
        "TiltedScript",
        "TiltedConnect")
    add_packages(
        "tiltedcore",
        "gamenetworkingsockets",
        "spdlog",
        "hopscotch-map")
end

build_server("SkyrimTogetherServer", "TP_SKYRIM=1")
build_server("FalloutTogetherServer", "TP_FALLOUT=1")