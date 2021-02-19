local uiBasePath = premake.extensions.ui.path
local coreBasePath = premake.extensions.core.path
local connectBasePath = premake.extensions.connect.path
local scriptBasePath = premake.extensions.script.path

function generate_client(target, macro)
    project (target)
        kind ("SharedLib")
        language ("C++")

        includedirs
        {
            "../Code/client/include/",
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
            premake.extensions.discord_dir .. "/c"
        }

        files
        {
            "../Code/client/**.h",
            "../Code/client/**.cpp",
            "../Libraries/spdlog/*.cpp",
            "../Code/encoding/**.h",
            "../Code/encoding/**.cpp",
        }

        if target == "FalloutTogether" then
            excludes 
            { 
                "../Code/client/Games/Skyrim/**.cpp",
                "../Code/client/Games/Skyrim/**.h",
                "../Code/encoding/Structs/Skyrim/**.h",
                "../Code/encoding/Structs/Skyrim/**.cpp",
            }

            includedirs
            {
                "../Code/client/Games/Fallout4/"
            }
        else
            excludes 
            { 
                "../Code/client/Games/Fallout4/**.cpp",
                "../Code/client/Games/Fallout4/**.h",
                "../Code/encoding/Structs/Fallout4/**.h",
                "../Code/encoding/Structs/Fallout4/**.cpp",
            }

            includedirs
            {
                "../Code/client/Games/Skyrim/"
            }
        end
        
        pchheader ("stdafx.h")
        pchsource ("../Code/client/stdafx.cpp")
        forceincludes
        {
            "stdafx.h"
        }
        
        links
        {
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
end

generate_client("FalloutTogether", "TP_FALLOUT=1")
generate_client("SkyrimTogether", "TP_SKYRIM=1")
