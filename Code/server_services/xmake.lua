add_requires("cargo::cxx 1.0")

target("ServerServices")
    set_kind("static")
    add_files("**.rs")
    set_values("rust.cratetype", "staticlib")
    add_packages("cargo::cxx")