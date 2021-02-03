local coreBasePath = premake.extensions.core.path

project ("Common")
    kind ("StaticLib")
    language ("C++")

    filter{}

    includedirs
    {
        "../Code/common/",
        "../Libraries/",
        coreBasePath .. "/Code/core/include/",
    }

    files 
    {
        "../Code/common/**.h",
        "../Code/common/**.cpp"
    }

    links
    {
        "Core"
    }
