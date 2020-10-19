local coreBasePath = premake.extensions.core.path

project ("Tests")
    kind ("ConsoleApp")
    language ("C++")

    defines { "TP_SKYRIM=1" }
    
    includedirs
    {
        "../Code/tests/include/",
        "../Code/encoding/include/",
        "../Libraries/entt/src/",
        coreBasePath .. "/Code/core/include/",
        coreBasePath .. "/ThirdParty"
    }

    files
    {
        "../Code/tests/include/**.h",
        "../Code/tests/src/**.cpp",
        "../Code/encoding/include/**.h",
        "../Code/encoding/src/**.cpp",
    }
    
    links
    {
        "Core",
        "mimalloc"
    }
