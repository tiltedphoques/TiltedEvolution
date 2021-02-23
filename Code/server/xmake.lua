
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
        "../../Libraries/cpp-httplib")
    set_pcxxheader("stdafx.h")
    add_headerfiles("**.h")
    add_files("**.cpp")

    if name == "SkyrimTogetherServer" then
        add_deps("SkyrimEncoding")
        del_files("../encoding/Structs/Fallout4/**.cpp")
    end
    if name == "FalloutTogetherServer" then
        add_deps("FalloutEncoding")
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
        "hopscotch-map",
        "sqlite3",
        "lua",
        "sol2",
        "glm",
        "entt")
end

build_server("SkyrimTogetherServer", "TP_SKYRIM=1")
build_server("FalloutTogetherServer", "TP_FALLOUT=1")
