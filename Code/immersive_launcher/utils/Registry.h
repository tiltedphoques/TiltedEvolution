#pragma once

#include <Windows.h>
#include <string>

namespace Registry
{
    template<typename T>
    inline std::basic_string<T> ReadString(HKEY aLocation, const wchar_t* aSubKey, const wchar_t* aKey)
    {
        std::basic_string<T> value{};

        DWORD dataSize{};
        LSTATUS result = ::RegGetValueW(aLocation, aSubKey, aKey, RRF_RT_REG_SZ, nullptr, nullptr, &dataSize);

        if (result != ERROR_SUCCESS || dataSize == 0)
            return value;

        // since RegGetValueW already gives us the size + one null terminated character, we have
        // to remove it, since std::strings maintain their own
        // see:
        // https://docs.microsoft.com/de-de/archive/msdn-magazine/2017/may/c-use-modern-c-to-access-the-windows-registry
        value.resize(dataSize / sizeof(T));

        ::RegGetValueW(aLocation, aSubKey, aKey, RRF_RT_REG_SZ, nullptr, &value[0], &dataSize);

        value.resize((dataSize - sizeof(T)) / sizeof(T));
        return value;
    }

    template <typename T>
    inline bool WriteString(HKEY aLocation, const wchar_t* aSubKey, const wchar_t* aKey, const std::basic_string<T> &aData)
    {
        HKEY hKey = nullptr;

        ::RegOpenKeyExW(aLocation, aSubKey, 0, KEY_WRITE, &hKey);

        if (!hKey)
            ::RegCreateKeyW(aLocation, aSubKey, &hKey);

        if (hKey)
        {
            LSTATUS ls = ::RegSetValueExW(hKey, aKey, 0, REG_SZ, 
                reinterpret_cast<const BYTE*>(aData.data()), 
                static_cast<DWORD>(aData.size() * sizeof(T)) + sizeof(T));

            ::RegCloseKey(hKey);
            return ls == ERROR_SUCCESS;
        }

        return false;
    }
}
