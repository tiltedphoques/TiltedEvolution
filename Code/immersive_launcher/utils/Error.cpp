
#include <Windows.h>
#include <spdlog/formatter.h>

#include "Utils/Error.h"


TiltedPhoques::String WinErrorToStringA(uint32_t aErrorCode)
{
    if (aErrorCode == 0)
    {
        return "";
    }

    char* pBuffer = nullptr;
    const size_t size =
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, aErrorCode, 0, reinterpret_cast<LPSTR>(&pBuffer), 0, nullptr);

    TiltedPhoques::String message(pBuffer, size);
    LocalFree(pBuffer);

    return message;
}

void Die(const char* aText)
{
    auto fmt = fmt::format("{}\nError: {} = {}", aText, GetLastError(), WinErrorToStringA(GetLastError()).c_str());
    MessageBoxA(nullptr, fmt.c_str(), "TiltedOnline", MB_ICONSTOP);
}
