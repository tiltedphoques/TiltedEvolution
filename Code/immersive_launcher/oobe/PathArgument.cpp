#include "PathArgument.h"

#include "TargetConfig.h"

#include "Utils/Error.h"
#include "utils/Registry.h"

#include <filesystem>

namespace oobe
{
using namespace TiltedPhoques;

namespace
{
constexpr wchar_t kTiltedRegistryPath[] = LR"(Software\TiltedPhoques\TiltedEvolution\)" SHORT_NAME;

#define DIE_NOW(err)    \
    {                   \
        Die(err, true); \
        return false;   \
    }

bool ValidatePath(const std::wstring& acPath)
{
    const std::wstring titlePath = acPath.substr(0, acPath.find_last_of('\\'));
    std::wstring errorText{};

    if (acPath.find_last_of('\\') == std::string::npos || acPath.ends_with(*"\\"))
    {
        SetLastError(ERROR_BAD_PATHNAME);
        errorText += L"Invalid path\n";
    }

    if (!acPath.ends_with(L".exe"))
    {
        SetLastError(ERROR_BAD_ARGUMENTS);
        errorText += acPath.substr(acPath.find_last_of('\\') + 1, acPath.back()) + L" is not an executable file\n";
    }
    else if (!acPath.ends_with(TARGET_NAME L".exe"))
    {
        SetLastError(ERROR_FILE_NOT_FOUND);
        errorText += TARGET_NAME L".exe not found\n";
    }


    if (!std::filesystem::exists(acPath) || !std::filesystem::exists(titlePath))
    {
        SetLastError(ERROR_BAD_PATHNAME);
        errorText += L"Path does not exist\n";
    }

    if (!errorText.empty())
    {
        errorText += L"\nPath: " + acPath;
        DIE_NOW(errorText.c_str())
    }

    return true;
}
} // namespace

bool PathArgument(const std::string& acPath)
{
    const std::wstring exePath = std::wstring(acPath.begin(), acPath.end());
    const std::wstring titlePath = exePath.substr(0, exePath.find_last_of('\\'));

    if (!ValidatePath(exePath))
    {
        DIE_NOW(L"Failed to validate path")
    }

    // Write to registry so oobe::SelectInstall can handle the rest
    const bool result = Registry::WriteString<wchar_t>(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitlePath", titlePath) && Registry::WriteString<wchar_t>(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitleExe", exePath);

    if (!result)
    {
        DIE_NOW(L"Failed to write to registry")
    }

    return true;
}
} // namespace oobe
