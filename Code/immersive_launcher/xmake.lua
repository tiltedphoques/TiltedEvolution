local function istable(t) return type(t) == 'table' end

local function build_launcher()
    set_kind("binary")
    set_group("Client")
    set_symbols("debug", "hidden")

    add_ldflags(
        "/FORCE:MULTIPLE",
        "/IGNORE:4254,4006",
        "/DYNAMICBASE:NO",
        "/SAFESEH:NO",
        "/LARGEADDRESSAWARE",
        "/INCREMENTAL:NO",
        "/LAST:.zdata",
        "/SUBSYSTEM:WINDOWS",
        "/ENTRY:mainCRTStartup", { force = true })
    add_includedirs(
        ".",
        "../",
        "../../Libraries/")
    add_headerfiles("**.h")
    add_files(
        "**.cpp",
        "launcher.rc")
    add_deps(
        "ImmersiveElf",
        "TiltedReverse",
        "TiltedHooks",
        "TiltedUi",
        "ImGuiImpl",
        "CommonLib")
    add_links("ntdll_x64")
    add_linkdirs(".")
    add_syslinks(
        "user32",
        "shell32",
        "comdlg32",
        "bcrypt",
        "ole32",
        "dxgi",
        "d3d11",
        "gdi32",
        "SetupAPI",
        "Powrprof",
        "Cfgmgr32",
        "Propsys",
        "delayimp")

    add_packages(
        "tiltedcore",
        "spdlog",
        "minhook",
        "hopscotch-map",
        "cryptopp",
        "glm",
        "cef",
        "mem")
end

local kGame = get_config("game") or "skyrim"
local kClientLib = kGame == "fallout4" and "Fallout4TogetherClient" or "SkyrimTogetherClient"
local kClientLibFile = kGame == "fallout4" and "FTClient" or "SkyrimTogetherClient"
local kLauncherName = kGame == "fallout4" and "Fallout4ImmersiveLauncher" or "SkyrimImmersiveLauncher"
local kBaseName = kGame == "fallout4" and "FalloutTogether" or "SkyrimTogether"

target(kLauncherName)
    set_basename(kBaseName)
    add_defines("TARGET_PREFIX=\"st\"")
    add_deps(kClientLib)
    add_ldflags("/WHOLEARCHIVE:" .. kClientLibFile, { force = true })
    build_launcher()
