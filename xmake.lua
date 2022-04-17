set_xmakever("2.6.2")

add_repositories("tmprepo tmprepo")

-- c code will use c99,
set_languages("c99", "cxx20")

set_arch("x64")
set_warnings("all")
add_vectorexts("sse", "sse2", "sse3", "ssse3")

-- build configurations
add_rules("mode.debug", "mode.releasedbg")

if has_config("unitybuild") then
    add_rules("c.unity_build")
    add_rules("c++.unity_build", {batchsize = 12})
end

add_requires("entt", "recastnavigation")

if is_plat("windows") then
    add_cxflags("/bigobj")
end

before_build(function (target)
    import("modules.version")
    local branch, commitHash = version()
    bool_to_number={ [true]=1, [false]=0 }
    local contents = string.format([[
    #pragma once
    #define IS_MASTER %d
    #define IS_BRANCH_BETA %d
    #define IS_BRANCH_PREREL %d
    ]], 
    bool_to_number[branch == "master"], 
    bool_to_number[branch == "bluedove"], 
    bool_to_number[branch == "prerel"])
    io.writefile("build/BranchInfo.h", contents)
end)

if is_mode("debug") then
    add_defines("DEBUG")
end

if is_plat("windows") then
    add_defines("NOMINMAX")
end

-- add projects
includes("Libraries")
includes("Code")

task("upload-symbols")
    on_run(function ()
        import("core.base.option")

        local key = option.get('key')

        if key ~= nil then
            import("net.http")
            import("core.project.config")

            config.load()

            local sentrybin = path.join(os.projectdir(), "build", "sentry-cli.exe")
            local path = path.join(os.projectdir(), "build", config.get("plat"), config.get("arch"), config.get("mode"), "SkyrimTogether.pdb")
            
            if not os.exists(sentrybin) then 
                http.download("https://github.com/getsentry/sentry-cli/releases/download/2.0.2/sentry-cli-Windows-x86_64.exe", sentrybin)
            end
            
            os.execv(sentrybin, {"--auth-token", key, "upload-dif", "-o", "together-team", "-p", "st-reborn", path})

        else
            print("An API key is required to proceed!")
        end
    end)

    set_menu {
        usage = "xmake upload-symbols",
        description = "Upload symbols to sentry",
        options = {
            {'k', "key", "kv", nil, "The API key to use." }
        }
    }