
local function build_server()
    set_kind("shared")
    set_group("Server")
    add_includedirs(
        ".",
        "../../Libraries/")
    set_pcxxheader("Pch.h")
    add_headerfiles("**.h")
    add_files("**.cpp")
    if is_plat("windows") then
        add_files("server.rc")
    end
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
    add_defines(
        "TARGET_ST",
        "TP_SKYRIM=1",
        "TARGET_PREFIX=\"st\"")
    add_deps("SkyrimEncoding")
    build_server()

--target("FalloutTogetherServer")
--    add_defines("TP_FALLOUT=1")
--    add_deps("FalloutEncoding")
--    build_server()
