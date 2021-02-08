
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
        "../encoding/**.h|../encoding/Structs/Skyrim/**.h|../encoding/Structs/Fallout/**.h")
    add_headerfiles("**.h")
    add_files(
        "**.cpp",
        "../../Libraries/spdlog/*.cpp",
        "../encoding/**.cpp|../encoding/Structs/Skyrim/**.cpp|../encoding/Structs/Fallout/**.cpp")

    if name == "ST" then
        add_headerfiles("../encoding/Structs/Skyrim/**.h")
        add_files("../encoding/Structs/Skyrim/**.cpp")
    end

    if name == "FT" then
        add_headerfiles("../encoding/Structs/Fallout4/**.h")
        add_files("../encoding/Structs/Fallout4/**.cpp")
    end

    set_pcxxheader("stdafx.h")
    add_deps(
        "Common",
        "TiltedScript",
        "TiltedConnect")
    add_packages(
        "tiltedcore",
        "gamenetworkingsockets",
        "hopscotch-map")
end

build_server("ST", "TP_SKYRIM=1")