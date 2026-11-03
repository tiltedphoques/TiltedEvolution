
#include <ScriptExtender.h>
#include <ScriptExtenderInfo.h>

#include <Windows.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/xchar.h>
#include <string>

#pragma comment(lib, "version.lib")

namespace
{
HMODULE s_scriptExtenderModule{nullptr};

int GetFileVersion(const std::filesystem::path& acFilePath, std::array<uint32_t, 4>& aVersion)
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
    if (!VerQueryValueA(&buf[0], "\\", reinterpret_cast<LPVOID*>(&pvi), reinterpret_cast<unsigned int*>(&sz)) || sz < sizeof(VS_FIXEDFILEINFO) ||
        pvi->dwSignature != VS_FFI_SIGNATURE)
    {
        return 3;
    }

    aVersion[0] = pvi->dwFileVersionMS >> 16;
    aVersion[1] = pvi->dwFileVersionMS & 0xFFFF;
    aVersion[2] = pvi->dwFileVersionLS >> 16;
    aVersion[3] = pvi->dwFileVersionLS & 0xFFFF;

    return 0;
}

std::wstring GetCoreFilename(const std::array<uint32_t, 4>& acRuntimeVersion)
{
    std::wstring filename = fmt::format(L"{}_{}_{}_{}", ScriptExtender::kDllPrefix, acRuntimeVersion[0], acRuntimeVersion[1], acRuntimeVersion[2]);
    if (acRuntimeVersion[3] != 0)
        filename += fmt::format(L"_{}", acRuntimeVersion[3]);
    return filename + L".dll";
}
} // namespace

bool IsScriptExtenderLoaded()
{
    return s_scriptExtenderModule != nullptr;
}

void LoadScriptExtender(const std::filesystem::path& acGamePath, const std::array<uint32_t, 4>& acRuntimeVersion)
{
    if (s_scriptExtenderModule)
        return;

    const auto path = acGamePath / GetCoreFilename(acRuntimeVersion);
    std::error_code error;
    if (!std::filesystem::is_regular_file(path, error))
    {
        spdlog::info("{} is not installed for this runtime; continuing without the script extender", ScriptExtender::kName);
        return;
    }

    std::array<uint32_t, 4> version{};
    if (GetFileVersion(path, version) != 0)
    {
        spdlog::error("Unable to verify {} version", ScriptExtender::kName);
        return;
    }

    if (version < ScriptExtender::kMinimumFileVersion)
    {
        spdlog::error("{} version is too old for this runtime", ScriptExtender::kName);
        return;
    }

    const auto module = LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!module)
    {
        spdlog::error("Failed to load {} (error {})", ScriptExtender::kName, GetLastError());
        return;
    }

    const auto pStart = reinterpret_cast<void (*)()>(GetProcAddress(module, ScriptExtender::kEntrypoint));
    if (!pStart)
    {
        spdlog::error("{} does not export {}", ScriptExtender::kName, ScriptExtender::kEntrypoint);
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
    // Matches MAKE_EXE_VERSION_EX in F4SE's public f4se_version.h.
    const uint32_t runtime = ((acRuntimeVersion[0] & 0xFF) << 24) | ((acRuntimeVersion[1] & 0xFF) << 16) | ((acRuntimeVersion[2] & 0xFFF) << 4) | (acRuntimeVersion[3] & 0xF);
    if (!pCore || pCore->dataVersion != 1 || pCore->runtimeVersion != runtime)
    {
        spdlog::error("F4SE runtime does not match the loaded Fallout 4 executable");
        FreeLibrary(module);
        return;
    }
#endif

    s_scriptExtenderModule = module;
    pStart();
    spdlog::info("{} {}.{}.{}.{} startup hooks installed", ScriptExtender::kName, version[0], version[1], version[2], version[3]);
}
