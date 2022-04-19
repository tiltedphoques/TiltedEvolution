local function istable(t) return type(t) == 'table' end

local function build_server()
    add_includedirs(
        ".",
        "../../Libraries/")

    after_install(function(target)
        local linkdir = target:pkg("sentry-native"):get("linkdirs")
        if istable(linkdir) then
            linkdir = linkdir[1] -- Yes lua index starts at 1
        end
        local bindir = path.join(linkdir, "..", "bin")
        os.cp(bindir, target:installdir())
    end)

    set_pcxxheader("stdafx.h")
    add_headerfiles("**.h")
    add_files(
        "**.cpp")
    if is_plat("windows") then
        add_files("server.rc")
    end
    add_deps(
        "CommonLib",
        "Console",
        "ESLoader",
        "CrashHandler",
        "BaseLib",
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
        "tiltedcore",
        "sentry-native")
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