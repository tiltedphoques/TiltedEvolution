local uiBasePath = premake.extensions.ui.path
local coreBasePath = premake.extensions.core.path
local connectBasePath = premake.extensions.connect.path
local scriptBasePath = premake.extensions.script.path

function generate_client(target, macro)
    project (target)
        kind ("SharedLib")
        language ("C++")

        defines { macro }
        
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
            premake.extensions.discord_dir .. "/c"
        }

        files
        {
            "../Code/client/include/**.h",
            "../Code/client/src/**.cpp",
            "../Libraries/spdlog/*.cpp",
            "../Code/encoding/include/**.h",
            "../Code/encoding/src/**.cpp",
        }

        if target == "FalloutTogether" then
            excludes 
            { 
                "../Code/client/src/Games/Skyrim/**.cpp",
                "../Code/client/include/Games/Skyrim/**.h",
            }

            includedirs
            {
                "../Code/client/include/Games/Fallout4/"
            }
        else
            excludes 
            { 
                "../Code/client/src/Games/Fallout4/**.cpp",
                "../Code/client/include/Games/Fallout4/**.h",
            }

            includedirs
            {
                "../Code/client/include/Games/Skyrim/"
            }
        end
        
        pchheader ("stdafx.h")
        pchsource ("../Code/client/src/stdafx.cpp")
        forceincludes
        {
            "stdafx.h"
        }
        
        links
        {
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
end

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

generate_client("FalloutTogether", "TP_FALLOUT=1")
generate_client("SkyrimTogether", "TP_SKYRIM=1")
