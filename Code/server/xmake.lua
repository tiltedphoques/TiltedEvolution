
local function build_server()
    set_kind("shared")
    set_group("Server")
    add_includedirs(
        ".",
        "../../Libraries/")
    set_pcxxheader("stdafx.h")
    add_headerfiles("**.h")
    add_files("**.cpp")

    add_deps(
        "Common",
        "Console",
        "ESLoader",
        "Base",
        "AdminProtocol",
        "TiltedScript",
        "TiltedConnect"
    )
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

target("SkyrimTogetherServer")
    add_defines("TP_SKYRIM=1")
    add_deps("SkyrimEncoding")
    build_server()

--target("FalloutTogetherServer")
--    add_defines("TP_FALLOUT=1")
--    add_deps("FalloutEncoding")
--    build_server()