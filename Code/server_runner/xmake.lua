local function istable(t) return type(t) == 'table' end

local function build_runner()
    after_install(function(target)
        local linkdir = target:pkg("sentry-native"):get("linkdirs")
        if istable(linkdir) then
            linkdir = linkdir[1] -- Yes lua index starts at 1
        end
        local bindir = path.join(linkdir, "..", "bin")
        os.cp(bindir, target:installdir())
    end)

    set_kind("binary")
    set_group("Server")
    add_includedirs(
        ".",
        "../",
        "../../Libraries/")
    add_headerfiles("**.h")
    add_files(
        "**.cpp")
    if is_plat("windows") then
        add_files("server_runner.rc")
    end
    add_deps(
        "CommonLib",
        "Console",
        "BaseLib")
    add_packages(
        "tiltedcore",
        "spdlog",
        "hopscotch-map",
        "sentry-native",
        "libuv")
    add_defines("SPDLOG_HEADER_ONLY")
end

target("SkyrimServerRunner")
    set_basename("SkyrimTogetherServer")
    add_defines(
        "TARGET_ST",
        "TARGET_PREFIX=\"st\"")
    -- we want uniform names for both runner and dll under windows for
    -- scripting/ux reasons
    build_runner()
    -- core dll
    add_deps("SkyrimTogetherServer")

target("FalloutServerRunner")
    set_basename("FalloutTogetherServer")
    add_defines(
        "TARGET_FT",
        "TARGET_PREFIX=\"ft\"")
    -- we want uniform names for both runner and dll under windows for
    -- scripting/ux reasons
    build_runner()
    -- core dll
    add_deps("FalloutTogetherServer")
