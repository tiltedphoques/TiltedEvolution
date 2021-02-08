
local function build_client(name, def)
target("client_" .. name)
    set_kind("shared")
    set_group("Client")
    add_defines(def)
    add_includedirs(
        ".",
        "../../Libraries/",
        "../../Libraries/entt")
    set_pcxxheader("stdafx.h")
    add_files("**.cpp")
    -- hacky way!
    if name == "ST" then
        add_files("**.cpp|Games/Fallout4/**.cpp")
        add_headerfiles("**.h|Games/Fallout4/**.h")
    end
    if name == "FT" then
        add_files("**.cpp|Games/Skyrim/**.cpp")
        add_headerfiles("**.h|Games/Skyrim/**.h")
    end
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
        "spdlog", 
        "hopscotch-map",
        "discord",
        "cef")
end

build_client("ST", "TP_SKYRIM=1")