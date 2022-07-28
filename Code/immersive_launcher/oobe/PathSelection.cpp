// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <Windows.h>

#include <optional>
#include <shobjidl.h>

#include "TargetConfig.h"
#include "launcher.h"

#include "utils/ComUtils.h"
#include "utils/Registry.h"

namespace oobe
{
using namespace TiltedPhoques;

namespace
{
constexpr wchar_t kBethesdaRegistryPath[] = LR"(Software\Wow6432Node\Bethesda Softworks\)" SHORT_NAME;
constexpr wchar_t kTiltedRegistryPath[] = LR"(Software\TiltedPhoques\TiltedEvolution\)" SHORT_NAME;

std::wstring SuggestTitlePath()
{
    return Registry::ReadString<wchar_t>(HKEY_LOCAL_MACHINE, kBethesdaRegistryPath, L"installed path");
}

std::optional<std::wstring> OpenPathSelectionDialog2(const std::wstring& aPathSuggestion)
{
    // in case some other stuff already tried to unregister the global instance
    ComScope _;

    ComPtr<IFileOpenDialog> pFileDialog;
    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog,
                               reinterpret_cast<void**>(pFileDialog.GetAddressOf()));
    if (FAILED(hr))
        return std::nullopt;

    pFileDialog->SetTitle(L"Select the game executable (" TARGET_NAME L".exe)");

    static constinit COMDLG_FILTERSPEC rgSpec[] = {{L"Executables", L"*.exe"}};
    pFileDialog->SetFileTypes(1, &rgSpec[0]);

    // pre select suggested folder & exe
    if (!aPathSuggestion.empty())
    {
        ComPtr<IShellItem> pItem;
        if (SUCCEEDED(SHCreateItemFromParsingName(aPathSuggestion.c_str(), nullptr, IID_PPV_ARGS(&pItem))))
        {
            pFileDialog->SetFolder(pItem.Get());
            pFileDialog->SetFileName(TARGET_NAME L".exe");
        }
    }

    hr = pFileDialog->Show(nullptr);
    if (FAILED(hr))
        return std::nullopt;

    ComPtr<IShellItem> pItem;
    hr = pFileDialog->GetResult(pItem.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        PWSTR filePath;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

        if (SUCCEEDED(hr))
        {
            auto rval = filePath;
            CoTaskMemFree(filePath);
            return rval;
        }
    }

    return std::nullopt;
}
} // namespace

bool SelectInstall(bool aForceSelect)
{
    // separate, so a custom executable can be chosen for TP
    auto titlePath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitlePath");
    auto exePath = Registry::ReadString<wchar_t>(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitleExe");

    bool result = true;
    if (!std::filesystem::exists(titlePath) || !std::filesystem::exists(exePath) || aForceSelect)
    {
        constexpr int kSelectionAttempts = 3;

        for (int i = 0; i < kSelectionAttempts; i++)
        {
            if (auto path = OpenPathSelectionDialog2(SuggestTitlePath()))
            {
                size_t pos = path->find_last_of(L'\\');
                if (pos == std::string::npos)
                    return false;

                titlePath = path->substr(0, pos);
                exePath = (*path);

                // game is installed into our directory, or otherwise.
                if (titlePath == TiltedPhoques::GetPath())
                {
                    continue;
                }

                // if this fails, we try again
                result = Registry::WriteString(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitlePath", titlePath) &&
                         Registry::WriteString(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitleExe", exePath);

                if (result)
                    break;
            }
            else
                break;
        }
    }

    if (result)
    {
        auto* apContext = launcher::GetLaunchContext();
        apContext->gamePath = titlePath;
        apContext->exePath = exePath;
    }

    return result;
}
} // namespace oobe
