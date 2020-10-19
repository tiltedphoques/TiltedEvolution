local coreBasePath = premake.extensions.core.path

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
