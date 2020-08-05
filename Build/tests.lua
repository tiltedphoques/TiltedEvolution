local coreBasePath = premake.extensions.core.path

project ("Tests")
    kind ("ConsoleApp")
    language ("C++")
    
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
    }
    
    links
    {
        "Encoding",
        "Core",
        "mimalloc"
    }
