#include "PathArgument.h"

#include "TargetConfig.h"

#include "Utils/Error.h"
#include "utils/Registry.h"

#include <filesystem>

std::wstring g_titlePath;
std::wstring g_exePath;

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

    if (!std::filesystem::exists(g_exePath) || !std::filesystem::exists(g_titlePath))
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
    g_exePath = std::wstring(acPath.begin(), acPath.end());
    g_titlePath = g_exePath.substr(0, g_exePath.find_last_of('\\'));

    if (!ValidatePath(g_exePath))
    {
        DIE_NOW(L"Failed to validate path")
    }

    // Write to registry so oobe::SelectInstall can handle the rest
    bool result = Registry::WriteString<wchar_t>(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitlePath", g_titlePath) && Registry::WriteString<wchar_t>(HKEY_CURRENT_USER, kTiltedRegistryPath, L"TitleExe", g_exePath);

    if (!result)
    {
        DIE_NOW(L"Failed to write to registry")
    }

    return true;
}
} // namespace oobe
