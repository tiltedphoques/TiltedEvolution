
local function build_client(name, def)
target("client_" .. name)
    set_kind("shared")
    set_group("Client")
    add_defines(def)
    add_includedirs(
        ".",
        "../../Libraries/",
        "../../Libraries/entt")
    add_headerfiles("**.h")

    if name == "FT" then
        add_files("**.cpp|Games/Skyrim/**.cpp")
    end

    if name == "ST" then
        add_files("**.cpp|Games/Fallout4/**.cpp")
    end

    add_files("**.cpp")
    set_pcxxheader("stdafx.h")
    add_deps(
        "UiProcess", 
        "Common",
        "Encoding",
        "TiltedReverse",
        "TiltedHooks",
        "TiltedScript",
        "TiltedUi")
    add_packages(
        "tiltedcore", 
        "hopscotch-map",
        "discord",
        "cef")
end

build_client("ST", "TP_SKYRIM=1")