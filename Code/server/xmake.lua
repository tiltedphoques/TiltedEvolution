local function istable(t) return type(t) == 'table' end

add_requires("sol2 v3.3.0", {configs = {lua = "lua"}})

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
    if is_plat("linux") then
        add_cxxflags("-fvisibility=hidden")
    end
    add_deps(
        "CommonLib",
        "Console",
        "Resources",
        "ESLoader",
        "CrashHandler",
        "BaseLib",
        "AdminProtocol",
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
        "tiltedcore",
        "sentry-native")
end

target("SkyrimTogetherServer")
    set_basename("STServer")
    add_defines("TARGET_PREFIX=\"st\"")
    add_deps("SkyrimEncoding")
    build_server()
