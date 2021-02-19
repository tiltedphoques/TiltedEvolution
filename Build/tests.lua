local coreBasePath = premake.extensions.core.path

project ("Tests")
    kind ("ConsoleApp")
    language ("C++")

    defines { "TP_SKYRIM=1" }
    
    includedirs
    {
        "../Code/tests/",
        "../Code/encoding/",
        "../Libraries/entt/",
        coreBasePath .. "/Code/core/",
        coreBasePath .. "/ThirdParty"
    }

    files
    {
        "../Code/tests/**.h",
        "../Code/tests/**.cpp",
        "../Code/encoding/**.h",
        "../Code/encoding/**.cpp",
    }
    
    links
    {
        "Core",
        "mimalloc"
    }
