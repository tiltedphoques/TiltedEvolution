local function istable(t) return type(t) == 'table' end

local function build_runner()
    set_kind("binary")
    set_group("Server")
    set_symbols("debug", "hidden")
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
        "libuv")
    add_defines("SPDLOG_HEADER_ONLY")
end

target("SkyrimServerRunner")
    set_basename("SkyrimTogetherServer")
    add_defines("TARGET_PREFIX=\"st\"")
    -- we want uniform names for both runner and dll under windows for
    -- scripting/ux reasons
    build_runner()
    -- core dll
    add_deps("SkyrimTogetherServer")
