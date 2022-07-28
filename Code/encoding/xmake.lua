local function build_encoding(name, def)
target(name)
    set_kind("static")
    set_group("common")
    add_defines(def)
    add_includedirs(".", "../", {public = true})
    add_headerfiles("**.h|Structs/Fallout4/**|Structs/Skyrim/**", {prefixdir = "Encoding"})
    add_files("**.cpp|Structs/Fallout4/**|Structs/Skyrim/**")
    set_pcxxheader("EncodingPch.h")

    if is_plat("linux") then
        add_cxxflags("-fPIC")
    end    

    if name == "SkyrimEncoding" then
        add_files("Structs/Skyrim/**.cpp")
        add_headerfiles("Structs/Skyrim/**.h")
        add_includedirs("Structs/Skyrim")
    end
    if name == "FalloutEncoding" then
        add_files("Structs/Fallout4/**.cpp")
        add_headerfiles("Structs/Fallout4/**.h")
        add_includedirs("Structs/Fallout4")
    end

    add_packages("hopscotch-map", "glm", "tiltedcore")
end

build_encoding("SkyrimEncoding", "TP_SKYRIM=1")
build_encoding("FalloutEncoding", "TP_FALLOUT=1")
