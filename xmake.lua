set_xmakever("2.8.5")

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
add_rules("mode.debug", "mode.releasedbg", "mode.release")

if has_config("unitybuild") then
    add_rules("c.unity_build")
    add_rules("c++.unity_build", {batchsize = 12})
end

-- direct dependencies version pinning 
add_requires(
    "entt v3.10.0", 
    "recastnavigation v1.6.0", 
    "tiltedcore v0.2.7", 
    "cryptopp 8.9.0", 
    "spdlog v1.13.0", 
    "cpp-httplib 0.14.0",
    "gtest v1.14.0", 
    "mem 1.0.0", 
    "glm 0.9.9+8", 
    "sentry-native 0.7.1", 
    "zlib v1.3.1"
)
if is_plat("windows") then
    add_requires(
        "discord 3.2.1", 
        "imgui v1.89.7"
    )
end

-- dependencies' dependencies version pinning
add_requireconfs("*.mimalloc", { version = "2.2.4", override = true })
add_requireconfs("*.cmake", { version = "3.30.2", override = true })
add_requireconfs("*.openssl", { version = "1.1.1-w", override = true })
add_requireconfs("*.zlib", { version = "v1.3.1", override = true })
if is_plat("linux") then
    add_requireconfs("*.libcurl", { version = "8.7.1", override = true })
end

add_requireconfs("cpp-httplib", {configs = {ssl = true}})
add_requireconfs("sentry-native", { configs = { backend = "crashpad" } })
--[[
add_requireconfs("magnum", { configs = { sdl2 = true }})
add_requireconfs("magnum-integration",  { configs = { imgui = true }})
add_requireconfs("magnum-integration.magnum",  { configs = { sdl2 = true }})
add_requireconfs("magnum-integration.imgui", { override = true })
--]]

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

    -- fix always-compiles problem by updating the file only if content has changed.
    local filepath = "build/BranchInfo.h"
    local old_content = nil
    if os.exists(filepath) then
        old_content = io.readfile(filepath)
    end
    if old_content ~= contents then
        print("Updating file:", filepath)
        io.writefile(filepath, contents)
    end
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
