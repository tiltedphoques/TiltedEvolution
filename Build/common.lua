local coreBasePath = premake.extensions.core.path

project ("Encoding")
    kind ("StaticLib")
    language ("C++")
        
    filter {}
    
    includedirs
    {
        "../Code/encoding/include/",
        "../Libraries/",
        coreBasePath .. "/Code/core/include/",
    }

    files
    {
        "../Code/encoding/include/**.h",
        "../Code/encoding/src/**.cpp",
    }
    
    links
    {
        "Core",
        "mimalloc"
    }
    
    filter { "action:gmake*", "language:C++" }
        defines
        {
            'POSIX',
            'LINUX',
            'GNUC',
            'GNU_COMPILER',
        }
        
        links
        {
            "stdc++fs",
            "ssl",
            "crypto"
        }

    filter ""

project ("Common")
    kind ("StaticLib")
    language ("C++")

    filter{}

    includedirs
    {
        "../Code/common/include/",
        "../Libraries/",
        coreBasePath .. "/Code/core/include/",
    }

    files 
    {
        "../Code/common/include/**.h",
        "../Code/common/src/**.cpp"
    }

    links
    {
        "Core"
    }
