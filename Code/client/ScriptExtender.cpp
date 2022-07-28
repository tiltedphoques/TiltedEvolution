
#include <ScriptExtender.h>
#include <TiltedOnlinePCH.h>
#include <VersionDb.h>

namespace
{
constexpr wchar_t kScriptExtenderName[] =
#if TP_SKYRIM
    L"skse64"
#elif TP_FALLOUT
    L"f4se"
#endif
    ;

constexpr char kScriptExtenderEntrypoint[] =
#if TP_SKYRIM
"StartSKSE"
#elif TP_FALLOUT
"StartF4SE"
#endif
    ;

constexpr size_t kScriptExtenderNameLength = sizeof(kScriptExtenderName) / sizeof(wchar_t) - 1;

// AE+ only
// Use this to raise the SKSE baseline
constexpr int kSKSEMinBuild = 20100;

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
    // make sure newer than anniversary!
    auto exeBuild = VersionDb::Get().GetLoadedVersionString();
    std::replace(exeBuild.begin(), exeBuild.end(), '.', '_');

    // chop off empty patch numbers for instance "1.6.323.0 becomes "1_6_323"
    auto patchPos = exeBuild.find_last_of("_0");
    if (patchPos != std::string::npos)
    {
        exeBuild.erase(exeBuild.begin() + (patchPos - 1), exeBuild.end());
    }

    return exeBuild;
}
} // namespace

bool IsScriptExtenderLoaded()
{
    return g_SKSEModuleHandle;
}

void LoadScriptExender()
{
    const auto exeVerson{GetSKSEStyleExeVersion()};

    // Get the path of the game, where the Script Extender dll resides
    const auto gameDir = std::filesystem::current_path();

    std::list<std::filesystem::path> dllMatches;
    for (const auto& dirEntry : std::filesystem::directory_iterator(gameDir))
    {
        const auto& path = dirEntry.path();
        if (path.extension() != L".dll")
            continue;

        auto fileName = path.filename().wstring();
        if (fileName.length() < kScriptExtenderNameLength)
            continue;

        if (fileName.substr(0, kScriptExtenderNameLength) == kScriptExtenderName)
        {
            dllMatches.push_back(path);
        }
    }

    // and before you ask, no, they dont expose it via file version info
    std::filesystem::path* needle = nullptr;
    for (auto& match : dllMatches)
    {
        auto fname = match.filename().string();
        auto ptr = &fname[kScriptExtenderNameLength + 1];
        // make extra sure!
        if (std::strncmp(ptr, exeVerson.c_str(), exeVerson.length()) == 0)
        {
            needle = &match;
            break;
        }
    }

    if (!needle)
        return;

    FileVersion fileVersion;
    if (GetFileVersion(*needle, fileVersion) != 0)
    {
        spdlog::error("Unable to verify Script Extender version");
        return;
    }

    auto skseVersion = fmt::format("v{}.{}.{}.{}", fileVersion.versions[0], fileVersion.versions[1],
                                   fileVersion.versions[2], fileVersion.versions[3]);

#if TP_SKYRIM
    // nice try.
    int SkseVCum = fileVersion.versions[0] * 1000000 + fileVersion.versions[1] * 10000 + fileVersion.versions[2] * 100 +
                   fileVersion.versions[3];
    if (SkseVCum < kSKSEMinBuild)
    {
        spdlog::error("Pre anniversary Script Extender is unsupported");
        return;
    }
#endif

    if (g_SKSEModuleHandle = LoadLibraryW(needle->c_str()))
    {
        if (auto* pStartSKSE =
                reinterpret_cast<void (*)()>(GetProcAddress(g_SKSEModuleHandle, kScriptExtenderEntrypoint)))
        {
            spdlog::info(
                "Starting SKSE {}... be aware that messages that start without a colored [timestamp] prefix are "
                "logs from the "
                "Script Extender and its loaded mods.",
                skseVersion);

            pStartSKSE();
            spdlog::info("SKSE is active");
        }
        else
            spdlog::warn("SKSE dll doesn't expose StartSKSE(), it may be outdated.");
    }
    else
    {
        spdlog::error("Failed to load {}! Check your privileges or re-download the Script Extender files.",
                      needle->string());
    }
}
