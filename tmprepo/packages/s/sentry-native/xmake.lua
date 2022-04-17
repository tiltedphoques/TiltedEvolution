package("sentry-native")
    set_homepage("https://sentry.io")
    set_description("Sentry SDK for C, C++ and native applications.")

    set_urls("https://github.com/getsentry/sentry-native/releases/download/$(version)/sentry-native.zip",
             "https://github.com/getsentry/sentry-native.git")

    add_versions("0.4.15", "ae3ac4efa76d431d8734d7b0b1bf9bbedaf2cbdb18dfc5c95e2411a67808cf29")
    add_versions("0.4.4", "fe6c711d42861e66e53bfd7ee0b2b226027c64446857f0d1bbb239ca824a3d8d")
    add_patches("0.4.4", path.join(os.scriptdir(), "patches", "0.4.4", "zlib_fix.patch"), "1a6ac711b7824112a9062ec1716a316facce5055498d1f87090d2cad031b865b")

    add_deps("cmake")

    add_configs("backend", {description = "Set the backend of sentry to use", default = nil, type = "string"})

    if is_plat("windows") then
        add_syslinks("dbghelp", "winhttp", "shlwapi", "advapi32", "version")
    elseif is_plat("linux") then
        add_deps("libcurl")
        add_syslinks("dl", "pthread", "rt")
    elseif is_plat("android") then
        add_syslinks("dl", "log")
    elseif is_plat("macosx") then
        add_deps("libcurl")
        add_frameworks("CoreText", "CoreGraphics", "CoreFoundation", "Foundation")
        add_syslinks("bsm")
    end

    on_load("windows", function (package)
        if not package:config("shared") then
            package:add("defines", "SENTRY_BUILD_STATIC")
        end
    end)

    on_install("windows", "linux", "macosx", function (package) -- TODO: to enable android you will need to figure out the order of libs
        local opt = {}
        local configs = {}
        table.insert(configs, "-DSENTRY_BUILD_EXAMPLES=OFF")
        table.insert(configs, "-DSENTRY_BUILD_TESTS=OFF")
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))

        if package:config("backend") == "crashpad" then
            package:add("links", "sentry", "crashpad_client", "crashpad_util", "crashpad_minidump", "crashpad_handler_lib", "mini_chromium", "crashpad_tools", "crashpad_compat", "crashpad_snapshot")
        elseif package:config("backend") == "breakpad" then
            package:add("links", "sentry", "breakpad_client")
        end

        if package:config("shared") then
            table.insert(configs, "-DBUILD_SHARED_LIBS=ON")
            table.insert(configs, "-DSENTRY_BUILD_SHARED_LIBS=ON")
        else
            table.insert(configs, "-DBUILD_SHARED_LIBS=OFF")
            table.insert(configs, "-DSENTRY_BUILD_SHARED_LIBS=OFF")
        end

        if package:config("backend") ~= nil then
            table.insert(configs, "-DSENTRY_BACKEND=" .. package:config("backend"))
        elseif package:is_plat("linux") then -- linux defaults to breakpad
            package:add("links", "sentry", "breakpad_client")
        end

        if package:is_plat("windows") then
            opt.cxflags = { "/experimental:preprocessor-" } -- fixes <Windows SDK>\um\oaidl.h(487): error C2059: syntax error: '/'
            local vs_runtime = package:config("vs_runtime")
            table.insert(configs, "-DSENTRY_BUILD_RUNTIMESTATIC=" .. ((vs_runtime == "MT" or vs_runtime == "MTd") and "ON" or "OFF"))
        end
        import("package.tools.cmake").install(package, configs, opt)
   end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            void test(int args, char** argv) {
                sentry_options_t* options = sentry_options_new();
                sentry_init(options);
                sentry_shutdown();
            }
        ]]}, {includes = {"sentry.h"}}))
    end)
