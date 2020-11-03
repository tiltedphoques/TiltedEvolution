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
            "../Libraries/spdlog/*.cpp",
            "../Code/encoding/include/**.h",
            "../Code/encoding/src/**.cpp",
        }

        if target == "FalloutTogetherServer" then
            excludes 
            { 
                "../Code/encoding/include/Structs/Skyrim/**.h",
                "../Code/encoding/src/Structs/Skyrim/**.cpp",
            }
        else
            excludes 
            { 
                "../Code/encoding/include/Structs/Fallout4/**.h",
                "../Code/encoding/src/Structs/Fallout4/**.cpp",
            }
        end
        
        pchheader ("stdafx.h")
        pchsource ("../Code/server/src/stdafx.cpp")
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

