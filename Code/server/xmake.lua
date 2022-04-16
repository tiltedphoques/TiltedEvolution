
local function build_server()
    add_includedirs(
        ".",
        "../../Libraries/")
    set_pcxxheader("stdafx.h")
    add_headerfiles("**.h")
    add_files(
        "**.cpp",
        "server.rc")
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
    set_kind("binary")
    set_group("Server")
    add_defines(
        "TARGET_ST",
        "TP_SKYRIM=1",
        "TARGET_PREFIX=\"st\"")
    add_deps("SkyrimEncoding")
    build_server()

-- add_deps("FalloutEncoding")