
#include <ScriptExtender.h>
#include <TiltedOnlinePCH.h>
#include <VersionDb.h>

namespace
{
#if defined(TP_FALLOUT4)
constexpr wchar_t kScriptExtenderName[] = L"f4se";
constexpr char kScriptExtenderEntrypoint[] = "StartF4SE";
constexpr char kScriptExtenderLabel[] = "F4SE";
constexpr int kScriptExtenderMinBuild = 709;
#else
constexpr wchar_t kScriptExtenderName[] = L"skse64";
constexpr char kScriptExtenderEntrypoint[] = "StartSKSE";
constexpr char kScriptExtenderLabel[] = "SKSE";
constexpr int kScriptExtenderMinBuild = 20100;
#endif

HMODULE g_SKSEModuleHandle{nullptr};

struct FileVersion
{
    static constexpr uint8_t scVersionSize = 4;
    DWORD versions[scVersionSize];
};

int GetFileVersion(const std::filesystem::path& acFilePath, FileVersion& aVersion)
{
    const auto filename = acFilePath.c_str();

    DWORD dwHandle = 0, sz = GetFileVersionInfoSizeW(filename, &dwHandle);
    if (0 == sz)
    {
        return 1;
    }
    std::string buf(sz, '\0');
    if (!GetFileVersionInfoW(filename, dwHandle, sz, &buf[0]))
    {
        return 2;
    }
    VS_FIXEDFILEINFO* pvi;
    sz = sizeof(VS_FIXEDFILEINFO);
    if (!VerQueryValueA(&buf[0], "\\", reinterpret_cast<LPVOID*>(&pvi), reinterpret_cast<unsigned int*>(&sz)))
    {
        return 3;
    }

    aVersion.versions[0] = pvi->dwProductVersionMS >> 16;
    aVersion.versions[1] = pvi->dwFileVersionMS & 0xFFFF;
    aVersion.versions[2] = pvi->dwFileVersionLS >> 16;
    aVersion.versions[3] = pvi->dwFileVersionLS & 0xFFFF;

    return 0;
}

std::string GetSKSEStyleExeVersion()
{
    auto exeBuild = VersionDb::Get().GetLoadedVersionString();
    if (exeBuild.ends_with(".0"))
        exeBuild.resize(exeBuild.size() - 2);
    std::replace(exeBuild.begin(), exeBuild.end(), '.', '_');

    return exeBuild;
}
} // namespace

bool IsScriptExtenderLoaded()
{
    return g_SKSEModuleHandle;
}

void LoadScriptExtender()
{
    if (g_SKSEModuleHandle)
        return;

    const auto gameDir = std::filesystem::current_path();
    const auto exeVersion = GetSKSEStyleExeVersion();
    const auto filename = fmt::format(L"{}_{}.dll", kScriptExtenderName, std::wstring(exeVersion.begin(), exeVersion.end()));
    const auto path = gameDir / filename;
    std::error_code error;
    if (!std::filesystem::is_regular_file(path, error))
        return;

    FileVersion version{};
    if (GetFileVersion(path, version) != 0)
    {
        spdlog::error("Unable to verify {} version", kScriptExtenderLabel);
        return;
    }

    const auto build = version.versions[0] * 1000000 + version.versions[1] * 10000 + version.versions[2] * 100 + version.versions[3];
    if (build < kScriptExtenderMinBuild)
    {
        spdlog::error("{} version is too old for this runtime", kScriptExtenderLabel);
        return;
    }

    const auto module = LoadLibraryW(path.c_str());
    if (!module)
    {
        spdlog::error("Failed to load {} (error {})", path.string(), GetLastError());
        return;
    }

    const auto pStart = reinterpret_cast<void (*)()>(GetProcAddress(module, kScriptExtenderEntrypoint));
    if (!pStart)
    {
        spdlog::error("{} does not export {}", path.string(), kScriptExtenderEntrypoint);
        FreeLibrary(module);
        return;
    }

#if defined(TP_FALLOUT4)
    struct CoreVersion
    {
        uint32_t dataVersion;
        uint32_t runtimeVersion;
    };
    const auto* pCore = reinterpret_cast<const CoreVersion*>(GetProcAddress(module, "F4SECore_Version"));
    int major, minor, revision, patch;
    VersionDb::Get().GetLoadedVersion(major, minor, revision, patch);
    const uint32_t runtime = (major << 24) | (minor << 16) | (revision << 4) | patch;
    if (!pCore || pCore->dataVersion != 1 || pCore->runtimeVersion != runtime)
    {
        spdlog::error("F4SE runtime does not match the loaded Fallout 4 executable");
        FreeLibrary(module);
        return;
    }
#endif

    g_SKSEModuleHandle = module;
    pStart();
    spdlog::info("{} {}.{}.{}.{} startup hooks installed", kScriptExtenderLabel, version.versions[0], version.versions[1], version.versions[2], version.versions[3]);
}
