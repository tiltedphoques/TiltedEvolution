#include <TiltedOnlinePCH.h>

#include <ScriptExtender.h>

int GetFileVersion(const std::filesystem::path& acFilePath, FileVersion& aVersion)
{
    const auto filename = acFilePath.c_str();

    DWORD dwHandle, sz = GetFileVersionInfoSizeW(filename, &dwHandle);
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

void InjectScriptExtenderDll()
{  
    // Get the path of the game, where the Script Extender dll resides
    const std::filesystem::path gamePath = std::filesystem::current_path();

    // Find applicable DLLs
    std::list<std::filesystem::path> potentialDllFiles;
#if TP_SKYRIM
    const std::string dllPrefix = "skse";
#elif TP_FALLOUT
    const std::string dllPrefix = "f4se";
#endif
    const std::string dllExtension = ".dll";

    // Find matching files prefixes and extension in the game directory
    for (const auto& entry : std::filesystem::directory_iterator(gamePath))
    {
        const auto& fileName = entry.path().filename().string();
        if (fileName.size() >= (dllPrefix.size() + dllExtension.size()))
        {
            if (fileName.substr(0, dllPrefix.size()) == dllPrefix &&
                fileName.substr(fileName.size() - dllExtension.size(), fileName.size()) == dllExtension)
            {
                potentialDllFiles.push_back(entry.path());
            }
        }
    }

    // From all files, use the latest dll version
    FileVersion latestScriptExtenderDllVersion{};
    for (auto& version : latestScriptExtenderDllVersion.versions)
    {
        version = 0;
    }
    std::filesystem::path latestScriptExtenderDllVersionPath;
    auto hasApplicableScriptExtenderDll = false;

    // Loop over every potential DLL to find the latest one
    for (const auto& entry : potentialDllFiles)
    {
        FileVersion version;
        if (GetFileVersion(entry, version) == 0)
        {
            for (auto i = 0; i < FileVersion::scVersionSize; ++i)
            {
                if (version.versions[i] > latestScriptExtenderDllVersion.versions[i])
                {
                    latestScriptExtenderDllVersion = version;
                    latestScriptExtenderDllVersionPath = entry;
                    hasApplicableScriptExtenderDll = true;
                    break;
                }
                else if (version.versions[i] < latestScriptExtenderDllVersion.versions[i])
                {
                    break;
                }
            }
        }
    }

    // Inject the latest ScriptExtender DLL
    if (hasApplicableScriptExtenderDll)
    {
        const auto handle = LoadLibraryW(latestScriptExtenderDllVersionPath.c_str());
        if (!handle)
        {
            spdlog::error("Failed to inject {}! Check your privileges or re-download the Script Extender files.", latestScriptExtenderDllVersionPath.filename().string());
        }
        else
        {
            spdlog::info("Successfully injected {}!", latestScriptExtenderDllVersionPath.filename().string());
            spdlog::info("Be aware that messages that start without a colored [timestamp] prefix are logs from the Script Extender and its loaded mods.");
        }
    }
}
