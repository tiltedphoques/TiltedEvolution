local coreBasePath = premake.extensions.core.path
local connectBasePath = premake.extensions.connect.path
local scriptBasePath = premake.extensions.script.path

function generate_server(target, macro)
    project (target)
        kind ("ConsoleApp")
        language ("C++")

        defines { macro }
        
        includedirs
        {
            "../Code/server/",
            "../Code/script/",
            "../Code/encoding/",
            "../Code/common/include",
            "../Libraries/entt/",
            "../Libraries/",
            "../Libraries/cpp-httplib",
            coreBasePath .. "/Code/core/",
            connectBasePath .. "/Code/connect/",
            connectBasePath .. "/ThirdParty/GameNetworkingSockets/",
            connectBasePath .. "/ThirdParty/protobuf/",
            connectBasePath .. "/ThirdParty/openssl/include",
            scriptBasePath .. "/ThirdParty/lua/",
            scriptBasePath .. "/Code/script/",
        }

        files
        {
            "../Code/server/**.h",
            "../Code/server/**.cpp",
            "../Libraries/spdlog/*.cpp",
            "../Code/encoding/**.h",
            "../Code/encoding/**.cpp",
        }

        if target == "FalloutTogetherServer" then
            excludes 
            { 
                "../Code/encoding/Structs/Skyrim/**.h",
                "../Code/encoding/Structs/Skyrim/**.cpp",
            }
        else
            excludes 
            { 
                "../Code/encoding/Structs/Fallout4/**.h",
                "../Code/encoding/Structs/Fallout4/**.cpp",
            }
        end
        
        pchheader ("stdafx.h")
        pchsource ("../Code/server/stdafx.cpp")
        forceincludes
        {
            "stdafx.h"
        }
        
        links
        {
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
end

generate_server("SkyrimTogetherServer", "TP_SKYRIM=1")
generate_server("FalloutTogetherServer", "TP_FALLOUT=1")

