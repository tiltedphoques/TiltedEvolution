set_xmakever("2.6.5")

-- If newer version of xmake, remove ccache until it actually works
if set_policy ~= nil then
    set_policy("build.ccache", false)
end

-- c code will use c99,
set_languages("c99", "cxx20")

if is_plat("windows") then
    add_cxflags("/bigobj")
    add_syslinks("kernel32")
    set_arch("x64")
end

if is_plat("linux") then
    add_cxflags("-fPIC")
end

set_warnings("all")
add_vectorexts("sse", "sse2", "sse3", "ssse3")
add_vectorexts("neon")

-- build configurations
add_rules("mode.debug", "mode.releasedbg")

if has_config("unitybuild") then
    add_rules("c.unity_build")
    add_rules("c++.unity_build", {batchsize = 12})
end

add_requires(
    "entt", 
    "recastnavigation", 
    "tiltedcore", 
    "cryptopp", 
    "spdlog", 
    "cpp-httplib",
    "gtest", 
    "mem", 
    "glm", 
    "sentry-native", 
    "zlib")

add_requireconfs("cpp-httplib", {configs = {ssl = true}})
add_requireconfs("sentry-native", { configs = { backend = "crashpad" } })
add_requireconfs("magnum", { configs = { sdl2 = true }})
add_requireconfs("magnum-integration",  { configs = { imgui = true }})
add_requireconfs("magnum-integration.magnum",  { configs = { sdl2 = true }})
add_requireconfs("magnum-integration.imgui", { override = true })

if is_plat("windows") then
    add_requires(
        "discord", 
        "imgui", 
        "magnum", 
        "magnum-integration")
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
        local linux = option.get('linux')

        if key ~= nil then
            import("net.http")
            import("core.project.config")

            config.load()

            local sentrybin = path.join(os.projectdir(), "build", "sentry-cli.exe")
            if not os.exists(sentrybin) then 
                http.download("https://github.com/getsentry/sentry-cli/releases/download/2.0.2/sentry-cli-Windows-x86_64.exe", sentrybin)
            end

            if linux then
                -- linux server bins
                local file_path = path.join(os.projectdir(), "build", "linux", "x64", "SkyrimTogetherServer.debug")
                os.execv(sentrybin, {"--auth-token", key, "upload-dif", "-o", "together-team", "-p", "st-server", file_path})

                file_path = path.join(os.projectdir(), "build", "linux", "x64", "libSTServer.debug")
                os.execv(sentrybin, {"--auth-token", key, "upload-dif", "-o", "together-team", "-p", "st-server", file_path})
            end

            -- windows bins
            if not linux then
                local file_path = path.join(os.projectdir(), "build", config.get("plat"), config.get("arch"), config.get("mode"), "SkyrimTogether.pdb")
                os.execv(sentrybin, {"--auth-token", key, "upload-dif", "-o", "together-team", "-p", "st-reborn", file_path})

                file_path = path.join(os.projectdir(), "build", config.get("plat"), config.get("arch"), config.get("mode"), "SkyrimTogetherServer.pdb")
                os.execv(sentrybin, {"--auth-token", key, "upload-dif", "-o", "together-team", "-p", "st-server", file_path})

                file_path = path.join(os.projectdir(), "build", config.get("plat"), config.get("arch"), config.get("mode"), "STServer.pdb")
                os.execv(sentrybin, {"--auth-token", key, "upload-dif", "-o", "together-team", "-p", "st-server", file_path})
            end

        else
            print("An API key is required to proceed!")
        end
    end)

    set_menu {
        usage = "xmake upload-symbols",
        description = "Upload symbols to sentry",
        options = {
            {'k', "key", "kv", nil, "The API key to use." },
            {'l', "linux", "v", false, "Upload linux symbols that were manually copied." },
        }
    }
