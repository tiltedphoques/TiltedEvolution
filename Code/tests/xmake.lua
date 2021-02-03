
target("Tests")
    set_kind("binary")
    set_group("tests")
    add_defines("TP_SKYRIM=1")
    add_includedirs(".", "../")
    add_headerfiles("**.h")
    add_files("*.cpp")
    add_deps("TiltedCore")
    add_packages("mimalloc")
    
    --add_deps("Core", "mimalloc")
