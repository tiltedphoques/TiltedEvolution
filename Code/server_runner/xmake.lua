
local function build_runner()
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
        "Common",
        "Console",
        "Base")
    add_packages(
        "tiltedcore",
        "spdlog",
        "hopscotch-map")
    add_defines("SPDLOG_HEADER_ONLY")
end

target("SkyrimServerRunner")
    set_basename("SkyrimTogetherServer")
    add_defines(
        "TARGET_ST",
        "TARGET_PREFIX=\"st\"")
    build_runner()
    -- core dll
    add_deps("SkyrimTogetherServer")