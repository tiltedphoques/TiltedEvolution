local function build_encoding(name, def)
target(name)
    set_kind("static")
    set_group("common")
    add_defines(def)
    add_includedirs(".", "../", {public = true})
    add_headerfiles("**.h", {prefixdir = "Encoding"})
    add_files("**.cpp")
    add_packages("tiltedcore", "hopscotch-map", "glm")
end

build_encoding("SkyrimEncoding", "TP_SKYRIM=1")
build_encoding("FalloutEncoding", "TP_FALLOUT=1")
