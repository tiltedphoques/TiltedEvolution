local function build_encoding(name)
target(name)
    set_kind("static")
    set_group("common")
    add_includedirs(".", "../", {public = true})
    add_headerfiles("**.h|Structs/Skyrim/**", {prefixdir = "Encoding"})
    add_files("**.cpp|Structs/Skyrim/**")
    set_pcxxheader("EncodingPch.h")

    if is_plat("linux") then
        add_cxxflags("-fPIC")
    end    

    add_files("Structs/Skyrim/**.cpp")
    add_headerfiles("Structs/Skyrim/**.h")
    add_includedirs("Structs/Skyrim")

    add_packages("hopscotch-map", "glm", "tiltedcore")
end

build_encoding("SkyrimEncoding")
