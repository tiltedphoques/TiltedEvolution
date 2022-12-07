add_requires("pybind11")
--add_requires("lua")
-- {configs = {shared = false}}
target("PythonScriptingRuntime")
    set_group("ServerPlugins")
    set_kind("shared")
    set_targetdir("$(buildir)/$(plat)/$(arch)/$(mode)/plugins")
    add_includedirs(
        ".",
        "../.." -- main dir
    ) 
    set_pcxxheader("Pch.h")
    add_headerfiles("**.h")
    add_files("**.cpp")
    add_deps("SkyrimTogetherServer")
    add_packages("pybind11")