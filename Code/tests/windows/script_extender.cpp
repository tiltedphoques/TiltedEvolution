#include <catch2/catch.hpp>
#include <Windows.h>

#include <ScriptExtender.h>
#include <ScriptExtenderInfo.h>
#include <loader/PathRerouting.h>

#include <filesystem>

TEST_CASE("Script extender core loading and plugin dependencies", "[script_extender]")
{
    namespace fs = std::filesystem;

    wchar_t executable[MAX_PATH]{};
    REQUIRE(GetModuleFileNameW(nullptr, executable, MAX_PATH) != 0);
    const fs::path executableDirectory = fs::path(executable).parent_path();
    const fs::path fixtures = executableDirectory / L"script-extender-fixtures";
    const fs::path gameDirectory = fixtures / L"game-\u03A9";
    const fs::path plugins = gameDirectory / ScriptExtender::kPluginDirectory;

#if defined(TP_FALLOUT4)
    const std::array<uint32_t, 4> runtime{1, 11, 240, 0};
    const fs::path core = gameDirectory / L"f4se_1_11_240.dll";
#else
    const std::array<uint32_t, 4> runtime{1, 7, 104, 0};
    const fs::path core = gameDirectory / L"skse64_1_7_104.dll";
#endif

    std::error_code error;
    fs::create_directories(plugins, error);
    REQUIRE_FALSE(error);
    fs::remove(core, error);
    REQUIRE_FALSE(error);
    fs::copy_file(fixtures / L"ScriptExtenderFixtureDependency.dll", plugins / L"ScriptExtenderFixtureDependency.dll", fs::copy_options::overwrite_existing, error);
    REQUIRE_FALSE(error);

    // The dependency is intentionally absent from the executable and game roots.
    REQUIRE_FALSE(fs::exists(executableDirectory / L"ScriptExtenderFixtureDependency.dll", error));
    REQUIRE_FALSE(error);
    REQUIRE(GetModuleHandleW(L"ScriptExtenderFixtureDependency.dll") == nullptr);
    loader::InstallPathRouting(gameDirectory);
    // Explicit game-path lookup must keep working if something changes the CWD.
    REQUIRE(SetCurrentDirectoryW(executableDirectory.c_str()));

    LoadScriptExtender(gameDirectory, runtime);
    REQUIRE_FALSE(IsScriptExtenderLoaded());

    const auto rejectFixture = [&](const wchar_t* apName)
    {
        INFO("Rejecting incompatible core fixture");
        fs::copy_file(fixtures / apName, core, fs::copy_options::overwrite_existing, error);
        REQUIRE_FALSE(error);
        LoadScriptExtender(gameDirectory, runtime);
        REQUIRE_FALSE(IsScriptExtenderLoaded());
        REQUIRE(GetModuleHandleW(core.c_str()) == nullptr);
    };

    rejectFixture(L"ScriptExtenderFixtureOld.dll");
    rejectFixture(L"ScriptExtenderFixtureNoEntrypoint.dll");
#if defined(TP_FALLOUT4)
    rejectFixture(L"ScriptExtenderFixtureBadRuntime.dll");
    rejectFixture(L"ScriptExtenderFixtureBadSchema.dll");
#endif

    fs::copy_file(fixtures / L"ScriptExtenderFixtureValid.dll", core, fs::copy_options::overwrite_existing, error);
    REQUIRE_FALSE(error);
    LoadScriptExtender(gameDirectory, runtime);
    REQUIRE(IsScriptExtenderLoaded());

    HMODULE module = GetModuleHandleW(core.c_str());
    REQUIRE(module != nullptr);
    const auto getStartCount = reinterpret_cast<int (*)()>(GetProcAddress(module, "GetFixtureStartCount"));
    const auto getDependencyValue = reinterpret_cast<int (*)()>(GetProcAddress(module, "GetFixtureDependencyValue"));
    REQUIRE(getStartCount != nullptr);
    REQUIRE(getDependencyValue != nullptr);
    REQUIRE(getStartCount() == 1);
    REQUIRE(getDependencyValue() == 42);

    LoadScriptExtender(gameDirectory, runtime);
    REQUIRE(getStartCount() == 1);
}
