local uiBasePath = premake.extensions.ui.path
local coreBasePath = premake.extensions.core.path
local connectBasePath = premake.extensions.connect.path
local scriptBasePath = premake.extensions.script.path

project ("Client")
    kind ("SharedLib")
    language ("C++")
    
    filter { "configurations:Skyrim" }
        targetname ("SkyrimTogether")

    filter { "configurations:Fallout4" }
        targetname ("FalloutTogether")
        
    filter {}
    
    includedirs
    {
        "../Code/client/include/",
        "../Code/script/include/",
        "../Code/encoding/include/",
        "../Code/common/include",
        "../Libraries/entt/src/",
        "../Libraries/",
        coreBasePath .. "/Code/core/include/",
        coreBasePath .. "/ThirdParty/mimalloc/include/",
        "../Libraries/TiltedReverse/Code/reverse/include/",
        "../Libraries/TiltedUI/Code/ui/include/",
        "../Libraries/TiltedUI/ThirdParty/CEF/",
        "../Libraries/TiltedHooks/Code/hooks/include/",
        "../Libraries/TiltedReverse/ThirdParty/",
        connectBasePath .. "/Code/connect/include/",
        connectBasePath .. "/ThirdParty/GameNetworkingSockets/include/",
        connectBasePath .. "/ThirdParty/protobuf/src/",
        uiBasePath .. "/ThirdParty/imgui/",
        scriptBasePath .. "/ThirdParty/lua/",
        scriptBasePath .. "/Code/script/include/",
        dsdkDir .. "/c"
    }

    files
    {
        "../Code/client/include/**.h",
        "../Code/client/src/**.cpp",
        "../Libraries/spdlog/spdlog.cpp"
    }
    
    pchheader ("stdafx.h")
    pchsource ("../Code/client/src/stdafx.cpp")
    forceincludes
    {
        "stdafx.h"
    }
    
    links
    {
        "Encoding",
        "Common",
        "Core",
        "Reverse",
        "Hooks",
        "mhook",
        "UI",
        "disasm",
        "Connect",
        "SteamNet",
        "Lua",
        "mimalloc",
        "Script",
        "sqlite3",
        "imgui",
        "Version",
        "snappy"
    }


project ("tp_process")
    kind ("WindowedApp")
    language ("C++")
    
    includedirs
    {
        "../Code/tests/include/",
        coreBasePath .. "/Code/core/include/",
        "../Code/tp_process/include/",
        "../Libraries/TiltedUI/Code/ui_process/include/",
        "../Libraries/TiltedUI/ThirdParty/CEF/",
    }

    files
    {
        "../Code/tp_process/include/**.h",
        "../Code/tp_process/src/**.cpp",
    }
    
    links
    {
        "Core",
        "UIProcess"
    }
