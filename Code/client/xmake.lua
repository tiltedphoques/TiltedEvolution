
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
    add_cxxflags("-include $(projectdir)/code/client/stdafx.h")
    add_headerfiles("**.h|Games/Fallout4/**|Games/Skyrim/**")
    add_files("**.cpp|Games/Fallout4/**|Games/Skyrim/**")
    -- only include selected files
    if name == "ST" then
        add_files("Games/Skyrim/**.cpp")
        add_headerfiles("Games/Skyrim/**.h")
        -- rather hacky:
        add_includedirs("Games/Skyrim")
    end
    if name == "FT" then
        add_files("Games/Fallout4/**.cpp")
        add_headerfiles("Games/Fallout4/**.h")
        -- rather hacky:
        add_includedirs("Games/Fallout4")
    end
    add_deps(
        "UiProcess", 
        "Common",
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
end

build_client("ST", "TP_SKYRIM=1")