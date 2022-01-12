
local function build_server(name, def)
target(name)
    set_kind("binary")
    set_group("Server")
    --set_basename(name .. "RebornServer")
    set_languages("cxx17")
    add_defines(def)
    add_includedirs(
        ".",
        "../../Libraries/")
    set_pcxxheader("stdafx.h")
    add_headerfiles("**.h")
    add_files("**.cpp")

    if name == "SkyrimTogetherServer" then
        add_deps("SkyrimEncoding")
    end
    if name == "FalloutTogetherServer" then
        add_deps("FalloutEncoding")
    end

    add_deps(
        "Common",
        "Base",
        "TiltedScript",
        "TiltedConnect",
        "AdminProtocol")
    add_packages(
        "gamenetworkingsockets",
        "spdlog",
        "hopscotch-map",
        "sqlite3",
        "lua",
        "sol2",
        "glm",
        "entt",
        "cpp-httplib",
        "tiltedcore")
end

build_server("SkyrimTogetherServer", "TP_SKYRIM=1")
build_server("FalloutTogetherServer", "TP_FALLOUT=1")
