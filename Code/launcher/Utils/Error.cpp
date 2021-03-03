
#include <Windows.h>
#include <spdlog/formatter.h>

#include "Utils/Error.h"

TiltedPhoques::WString WinErrorToString(uint32_t aErrorCode)
{
    if (aErrorCode == 0)
    {
        return L"";
    }

    wchar_t* pBuffer = nullptr;
    const size_t size =
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, aErrorCode, 0, reinterpret_cast<LPWSTR>(&pBuffer), 0, nullptr);

    TiltedPhoques::WString message(pBuffer, size);
    LocalFree(pBuffer);

    return message;
}

void FatalError(const wchar_t* aText)
{
    auto fmt = fmt::format(L"{}\nError: {} = {}", 
        aText, GetLastError(), WinErrorToString(GetLastError()));

    MessageBoxW(nullptr, fmt.c_str(), L"TiltedOnline", MB_ICONSTOP);
}
