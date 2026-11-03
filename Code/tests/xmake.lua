
target("TPTests")
    set_kind("binary")
    set_group("Tests")
    add_includedirs(
        ".", "../encoding")
    add_headerfiles("**.h")
    add_files("*.cpp")
    -- Has no game dependencies, so the client's remote playback timing can be tested here
    add_files("../client/Systems/PlaybackClock.cpp")
    add_deps("SkyrimEncoding")
    add_packages(
        "tiltedcore",
        "hopscotch-map",
        "catch2",
        "mimalloc",
        "glm")

-- Explicit Windows loader checks, with no game executable or engine hooks.
if is_plat("windows") then
    local fixture_dir = path.join(os.projectdir(), get_config("builddir") or "build", "$(plat)", "$(arch)", "$(mode)", "script-extender-fixtures")

    target("ScriptExtenderFixtureDependency")
        set_kind("shared")
        set_default(false)
        set_targetdir(fixture_dir)
        add_files("windows/fixtures/script_extender_dependency.cpp")

    local fixtures = {
        {"Valid", nil},
        {"Old", "SCRIPT_EXTENDER_FIXTURE_OLD"},
        {"NoEntrypoint", "SCRIPT_EXTENDER_FIXTURE_NO_ENTRYPOINT"},
        {"BadRuntime", "SCRIPT_EXTENDER_FIXTURE_BAD_RUNTIME"},
        {"BadSchema", "SCRIPT_EXTENDER_FIXTURE_BAD_SCHEMA"}
    }
    local fixture_targets = {}
    for _, fixture in ipairs(fixtures) do
        local name = "ScriptExtenderFixture" .. fixture[1]
        table.insert(fixture_targets, name)
        target(name)
            set_kind("shared")
            set_default(false)
            set_targetdir(fixture_dir)
            add_files("windows/fixtures/script_extender.cpp", "windows/fixtures/script_extender.rc")
            add_deps("ScriptExtenderFixtureDependency")
            if fixture[2] then
                add_defines(fixture[2])
            end
    end

    target("ScriptExtenderTests")
        set_kind("binary")
        set_default(false)
        set_group("Tests")
        add_files("main.cpp", "windows/script_extender.cpp", "../client/ScriptExtender.cpp", "../immersive_launcher/loader/PathRerouting.cpp")
        add_includedirs("../client", "../common", "../immersive_launcher")
        for _, name in ipairs(fixture_targets) do
            add_deps(name, {inherit = false})
        end
        add_packages("catch2", "spdlog", "tiltedcore", "hopscotch-map", "mimalloc")
        add_syslinks("version")
end
