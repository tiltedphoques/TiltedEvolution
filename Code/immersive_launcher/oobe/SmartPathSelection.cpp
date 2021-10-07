// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <optional>
#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Filesystem.hpp>

#include "GameConfig.h"

#include "utils/Registry.h"
#include "utils/Error.h"
#include "Launcher.h"

namespace oobe
{
using namespace TiltedPhoques;

constexpr wchar_t kRegistryPath[] = LR"(Software\TiltedPhoques\TiltedEvolution\)";

static std::wstring SuggestTitlePath()
{
    auto path = WString(LR"(Software\Wow6432Node\Bethesda Softworks\)") + kGame.shortGameName;

    const wchar_t* subName =
#if defined(IS_SKYRIM_TYPE)
        L"installed path";
#else
        L"Installed Path";
#endif

    return Registry::ReadString<wchar_t>(HKEY_LOCAL_MACHINE, path.c_str(), subName);
}

static std::optional<std::wstring> OpenPathSelectionPromt(const std::wstring &aSuggestion)
{
    OPENFILENAMEW file{};

    std::wstring buffer;
    buffer.resize(MAX_PATH);

    file.lpstrFile = buffer.data();
    file.lStructSize = sizeof(file);
    file.nMaxFile = MAX_PATH;
    file.lpstrFilter = L"Executables\0*.exe\0";
    file.lpstrDefExt = L"EXE";
    file.lpstrTitle = L"Please select your Game executable (*.exe)";
    file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER;
    file.lpstrInitialDir = aSuggestion.empty() ? buffer.data() : aSuggestion.data();

    if (!GetOpenFileNameW(&file))
        return std::nullopt;

    return buffer;
}

bool SelectInstall(bool aForceSelect)
{
    const WString regPath = WString(kRegistryPath) + kGame.shortGameName;

    // separate, so a custom executable can be chosen for TP
    auto titlePath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, regPath.c_str(), L"TitlePath");
    auto exePath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, regPath.c_str(), L"TitleExe");

    bool result = true;
    if (!std::filesystem::exists(titlePath) || 
        !std::filesystem::exists(exePath) || aForceSelect)
    {
        if (auto path = OpenPathSelectionPromt(SuggestTitlePath()))
        {
            size_t pos = path->find_last_of(L'\\');
            if (pos == std::string::npos)
                return false;

            titlePath = path->substr(0, pos);
            exePath = (*path);

            result = Registry::WriteString(HKEY_CURRENT_USER, regPath.c_str(), L"TitlePath", titlePath) &&
                     Registry::WriteString(HKEY_CURRENT_USER, regPath.c_str(), L"TitleExe", exePath);
        }

        result = false;
    }

    if (result) 
    {
        GetLaunchContext()->gamePath = titlePath;
        GetLaunchContext()->exePath = exePath;
    }

    return result;
}
} // namespace oobe
