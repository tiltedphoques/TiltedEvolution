local coreBasePath = premake.extensions.core.path
local connectBasePath = premake.extensions.connect.path
local scriptBasePath = premake.extensions.script.path

project ("Server")
    kind ("ConsoleApp")
    language ("C++")
    
    filter { "configurations:Skyrim" }
        targetname ("SkyrimTogetherServer")

    filter { "configurations:Fallout4" }
        targetname ("FalloutTogetherServer")
        
    filter {}
    
    includedirs
    {
        "../Code/server/include/",
        "../Code/script/include/",
        "../Code/encoding/include/",
        "../Code/common/include",
        "../Libraries/entt/src/",
        "../Libraries/",
        "../Libraries/cpp-httplib",
        coreBasePath .. "/Code/core/include/",
        connectBasePath .. "/Code/connect/include/",
        connectBasePath .. "/ThirdParty/GameNetworkingSockets/include/",
        connectBasePath .. "/ThirdParty/protobuf/src/",
        connectBasePath .. "/ThirdParty/openssl/include",
        scriptBasePath .. "/ThirdParty/lua/",
        scriptBasePath .. "/Code/script/include/",
    }

    files
    {
        "../Code/server/include/**.h",
        "../Code/server/src/**.cpp",
        "../Libraries/spdlog/spdlog.cpp"
    }
    
    pchheader ("stdafx.h")
    pchsource ("../Code/server/src/stdafx.cpp")
    forceincludes
    {
        "stdafx.h"
    }
    
    links
    {
        "Encoding",
        "Common",
        "Connect",
        "snappy",
        "SteamNet",
        "Script",
        "Core",
        "mimalloc",
        "Lua",
        "sqlite3",
        "protobuf"
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

    filter {"action:vs*"}
        links
        {
            "libcryptoMT.lib",
            "libsslMT.lib",
            "Crypt32.lib"
        }

    filter ""
