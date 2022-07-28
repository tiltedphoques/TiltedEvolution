
#include <Windows.h>

#define SPDLOG_WCHAR_FILENAMES
#include <spdlog/formatter.h>

#include "TargetConfig.h"
#include "Utils/Error.h"
#include "utils/ComUtils.h"

namespace
{
TiltedPhoques::WString WinErrorToString(uint32_t aErrorCode)
{
    wchar_t* pBuffer = nullptr;
    const size_t size =
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, aErrorCode, 0, reinterpret_cast<wchar_t*>(&pBuffer), 0, nullptr);

    TiltedPhoques::WString message(pBuffer, size);
    LocalFree(pBuffer);

    return message;
}
} // namespace

void Die(const wchar_t* aText, bool aNow)
{
    DWORD ec = GetLastError();
    std::wstring fmt =
        ec == 0 ? aText
                : fmt::format(L"{}\nError: {} = {}", aText, GetLastError(), WinErrorToString(GetLastError()).c_str());
    MessageBoxW(nullptr, fmt.c_str(), PRODUCT_NAME, MB_ICONSTOP);

    if (aNow)
        TerminateProcess(GetCurrentProcess(), 5);
}

void ShowProgressDialog()
{
    // in case some other stuff already tried to unregister the global instance
    ComScope _;

    // ComPtr<IProgressDialog > pDialog;
}
