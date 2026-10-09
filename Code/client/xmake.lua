
local function build_client(name)
target(name)
    set_kind("static")
    set_group("Client")
    if get_config("game") == "fallout4" then
        set_basename("FTClient")
    end
    add_includedirs(".","../../Libraries/")
    set_pcxxheader("TiltedOnlinePCH.h")

    local kGame = get_config("game") or "skyrim"
    local kGameDir = kGame == "fallout4" and "Games/Fallout4" or "Games/Skyrim"

    -- exclude game specifc stuff
    add_headerfiles("**.h|Games/Skyrim/**|Games/Fallout4/**|Services/Vivox/**")
    add_files("**.cpp|Games/Skyrim/**|Games/Fallout4/**|Services/Vivox/**")

    after_install(function(target)
        -- copy dlls
        for _, pkg_with_dlls in ipairs({"cef", "discord"}) do
            local linkdir = target:pkg(pkg_with_dlls):get("linkdirs")
            local bindir = path.join(linkdir, "..", "bin")
            os.cp(bindir, target:installdir())
        end
        -- copy the overlay cursor of the game's ui theme
        local uidir = path.join(target:scriptdir(), "..", "skyrim_ui", "src")
        import("core.project.config")
        local imagedir = config.get("game") == "fallout4" and path.join(uidir, "themes", "fallout", "assets", "images") or path.join(uidir, "assets", "images")
        os.cp(path.join(imagedir, "cursor.dds"), path.join(target:installdir(), "bin", "assets", "images", "cursor.dds"))
        os.cp(path.join(imagedir, "cursor.png"), path.join(target:installdir(), "bin", "assets", "images", "cursor.png"))
        os.rm(path.join(target:installdir(), "bin", "**Tests.exe"))
    end)

    add_files(kGameDir .. "/**.cpp")
    add_headerfiles(kGameDir .. "/**.h")
    -- rather hacky:
    add_includedirs(kGameDir)
    add_deps("SkyrimEncoding")
    add_deps(
        "UiProcess",
        "CommonLib",
        "BaseLib",
        "ImGuiImpl",
        "TiltedConnect",
        "TiltedReverse",
        "TiltedHooks",
        "TiltedUi",
        {inherit = true}
    )

    add_packages(
        "tiltedcore",
        "spdlog",
        "hopscotch-map",
        "cryptopp",
        "gamenetworkingsockets",
        "discord",
        "imgui",
        "cef",
        "minhook",
        "entt",
        "glm",
        "mem",
        "xbyak")

    if has_config("vivox") then
        add_files("Services/Vivox/**.cpp")
        add_headerfiles("Services/Vivox/**.h")
        add_includedirs("Services/Vivox")
        add_deps("Vivox")
        add_defines("TP_VIVOX=1")
    else
        add_defines("TP_VIVOX=0")
    end

    add_syslinks(
        "version",
        "dbghelp",
        "kernel32")
end

add_requires("tiltedcore")

local kGame = get_config("game") or "skyrim"
build_client(kGame == "fallout4" and "Fallout4TogetherClient" or "SkyrimTogetherClient")
