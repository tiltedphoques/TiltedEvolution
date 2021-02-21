
local function build_client(name, def)
target(name)
    set_kind("shared")
    set_group("Client")
    --set_basename(name .. "RebornClient")
    add_defines(def)
    add_includedirs(
        ".",
        "../../Libraries/",
        "../../Libraries/entt")
    set_pcxxheader("TiltedOnlinePCH.h")

    -- exclude game specifc stuff
    add_headerfiles("**.h|Games/Fallout4/**|Games/Skyrim/**")
    add_files("**.cpp|Games/Fallout4/**|Games/Skyrim/**")

    after_install(function(target)
        local linkdir = target:pkg("cef"):get("linkdirs")
        local bindir = path.join(linkdir, "..", "bin")
        os.cp(bindir, target:installdir())
        os.rm(path.join(target:installdir(), "bin", "**Tests.exe"))
    end)

    -- only include selected files
    if name == "SkyrimTogether" then
        add_files("Games/Skyrim/**.cpp")
        add_headerfiles("Games/Skyrim/**.h")
        -- rather hacky:
        add_includedirs("Games/Skyrim")
    end
    if name == "FalloutTogether" then
        add_files("Games/Fallout4/**.cpp")
        add_headerfiles("Games/Fallout4/**.h")
        -- rather hacky:
        add_includedirs("Games/Fallout4")
    end
    add_deps(
        "UiProcess",
        "Common",
        "mhook",
        "Encoding",
        "TiltedConnect",
        "TiltedReverse",
        "TiltedHooks",
        "TiltedScript",
        "TiltedUi")

    add_packages(
        "tiltedcore",
        "spdlog",
        "hopscotch-map",
        "gamenetworkingsockets",
        "discord",
        "imgui",
        "cef")

    add_syslinks(
        "version")
end

build_client("SkyrimTogether", "TP_SKYRIM=1")
build_client("FalloutTogether", "TP_FALLOUT=1")
