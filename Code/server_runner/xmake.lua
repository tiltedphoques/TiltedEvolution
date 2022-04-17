
local function build_runner()
    set_kind("binary")
    set_group("Server")
    add_includedirs(
        ".",
        "../",
        "../../Libraries/")
    add_headerfiles("**.h")
    add_files("**.cpp")
    add_deps(
        "Common",
        "Console",
        "Base")
    add_packages(
        "tiltedcore",
        "spdlog",
        "hopscotch-map")
end

target("SkyrimServerRunner")
    set_basename("SkyrimTogetherServer")
    add_defines(
        "TARGET_ST",
        "TARGET_PREFIX=\"st\"")
    build_runner()