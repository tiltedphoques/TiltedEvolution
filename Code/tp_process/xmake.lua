
target("TpProcess")
    set_kind("binary")
    set_group("client")
    add_includedirs(".", "../")
    add_headerfiles("*.h")
    add_files("*.cpp")
    add_deps("TiltedCore")
    add_packages("mimalloc")
    
    --add_deps("Core", "mimalloc")
