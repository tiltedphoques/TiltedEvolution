
local function build_client(name, def)
target(name)
    set_kind("static")
    set_group("Client")
    add_defines(def)
    add_includedirs(".")
    set_pcxxheader("TiltedOnlinePCH.h")

    -- exclude game specifc stuff
    add_headerfiles("**.h|Games/Fallout4/**|Games/Skyrim/**|Services/Vivox/**")
    add_files("**.cpp|Games/Fallout4/**|Games/Skyrim/**|Services/Vivox/**")

    after_install(function(target)
        local linkdir = target:pkg("cef"):get("linkdirs")
        local bindir = path.join(linkdir, "..", "bin")
        local uidir = path.join(target:scriptdir(), "..", "skyrim_ui", "src")
        os.cp(bindir, target:installdir())
        os.cp(path.join(uidir, "assets", "images", "cursor.dds"), path.join(target:installdir(), "bin", "assets", "images", "cursor.dds"))
        os.cp(path.join(uidir, "assets", "images", "cursor.png"), path.join(target:installdir(), "bin", "assets", "images", "cursor.png"))
        os.rm(path.join(target:installdir(), "bin", "**Tests.exe"))
    end)

    -- only include selected files
    if name == "SkyrimTogetherClient" then
        add_files("Games/Skyrim/**.cpp")
        add_headerfiles("Games/Skyrim/**.h")
        -- rather hacky:
        add_includedirs("Games/Skyrim")
        add_deps("SkyrimEncoding")
    end
    if name == "FalloutTogetherClient" then
        add_files("Games/Fallout4/**.cpp")
        add_headerfiles("Games/Fallout4/**.h")
        -- rather hacky:
        add_includedirs("Games/Fallout4")
        add_deps("FalloutEncoding")
    end
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

add_requires("tiltedcore", {debug = true})

build_client("SkyrimTogetherClient", "TP_SKYRIM=1")
build_client("FalloutTogetherClient", "TP_FALLOUT=1")
