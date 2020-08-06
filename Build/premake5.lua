require("premake", ">=5.0.0-alpha10")

------
-- Prepare dependencies
------

include "setup.lua"

include "../Libraries/TiltedCore/Build/module.lua"
include "../Libraries/TiltedConnect/Build/module.lua"
include "../Libraries/TiltedScript/Build/module.lua"

if (os.istarget("Windows") == true) then
    include "../Libraries/TiltedReverse/Build/module.lua"
    include "../Libraries/TiltedUI/Build/module.lua"
    include "../Libraries/TiltedHooks/Build/module.lua"
end

workspace ("Tilted Online Framework")

    ------------------------------------------------------------------
    -- setup common settings
    ------------------------------------------------------------------
    configurations { "Skyrim", "Fallout4" }

    location ("projects")
    startproject ("Tests")
    
    staticruntime "On"
    vectorextensions "SSE2"
    warnings "Extra"
    
    flags { "MultiProcessorCompile" }
    
    cppdialect "C++17"

	platforms { "x64" }

    includedirs
    { 
        "../ThirdParty/", 
        "../Code/"
    }
    
    defines { "SPDLOG_COMPILED_LIB", "_CRT_SECURE_NO_WARNINGS", "SOL_NO_EXCEPTIONS", "CPPHTTPLIB_OPENSSL_SUPPORT" }
	
    filter { "action:vs*"}
        buildoptions { "/wd4512", "/wd4996",  "/wd4018", "/wd4100", "/wd4125", "/wd4214", "/wd4127", "/wd4005", "/wd4324", "/Zm500" }
        linkoptions { "/IGNORE:4099", "/IGNORE:4006" }
        defines { "WIN32" }
        
    filter { "action:gmake*", "language:C++" }
        buildoptions { "-g -fpermissive" }
        linkoptions ("-lm -lpthread -pthread -Wl,--no-as-needed -lrt -g -fPIC")

    filter { "configurations:Skyrim" }
        defines { "NDEBUG", "PUBLIC_BUILD", "TP_SKYRIM" }
        optimize ("On")
        
    filter { "configurations:Fallout4" }
        defines { "NDEBUG", "PUBLIC_BUILD", "TP_FALLOUT" }
        optimize ("On")

    filter { "architecture:*64" }
        libdirs { "lib/x64", "../Libraries/TiltedConnect/ThirdParty/openssl/lib/x64" }
        targetdir ("bin/x64")
        
    filter {}

    group ("Utilities")
        project ("_MakeProjects")
            kind ("Utility")
            
            postbuildcommands { 'cd "$(SolutionDir).." && MakeVS2019Projects.bat' }

    group ("Applications")
        
        include "common.lua"
        include "server.lua"
        include "tests.lua"

        if (os.istarget("Windows") == true) then
            include "client.lua"
        end
            
    premake.extensions.connect.generate()
    premake.extensions.script.generate()
    premake.extensions.core.generate()

    if (os.istarget("Windows") == true) then
        premake.extensions.ui.generate()
        premake.extensions.reverse.generate()
        premake.extensions.hooks.generate()
    end
    
