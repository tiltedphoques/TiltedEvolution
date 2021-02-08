
local function build_server(name, def)
target("server_" .. name)
    set_kind("binary")
    set_group("Server")
    set_languages("cxx17")
    add_defines(def)
    add_includedirs(
        ".",
        "../../Libraries/",
        "../../Libraries/entt",
        "../encoding/")
    set_pcxxheader("stdafx.h")
    add_cflags("-include $(projectdir)/code/server/stdafx.h")
    add_headerfiles(
        "**.h",
        "../encoding/**.h")
    add_files(
        "**.cpp",
        "../encoding/**.cpp")
        
    if name == "ST" then
        del_files("../encoding/Structs/Fallout4/**.cpp")
    end
    if name == "FT" then
        del_files("../encoding/Structs/Skyrim/**.cpp")
    end

    add_deps(
        "Common",
        "TiltedScript",
        "TiltedConnect")
    add_packages(
        "tiltedcore",
        "gamenetworkingsockets",
        "spdlog",
        "hopscotch-map")
end

build_server("ST", "TP_SKYRIM=1")