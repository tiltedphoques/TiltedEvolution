
target("TPTests")
    set_kind("binary")
    set_group("Tests")
    add_includedirs(
        ".", "../encoding", "../client")
    add_headerfiles("**.h")
    add_files("*.cpp")
    add_files("../client/Systems/ObjectSystem.cpp")
    add_deps("SkyrimEncoding")
    add_packages(
        "tiltedcore",
        "hopscotch-map",
        "catch2",
        "mimalloc",
        "glm",
        "entt")

option("server_tests")
    set_default(false)
    set_showmenu(true)
    set_description("Build isolated server object lifecycle tests")
option_end()

if has_config("server_tests") then
    includes("server")
end
