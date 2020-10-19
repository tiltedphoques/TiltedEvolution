#pragma once

struct BSFixedString
{
    BSFixedString();
    BSFixedString(const char* acpData);
    ~BSFixedString();

    void Set(const char* acpData);
    void Release() noexcept;

    operator const char* () const noexcept { return data; }

    [[nodiscard]] bool IsAscii() const noexcept { return true; }
    [[nodiscard]] const char* AsAscii() const noexcept { return data; }
    [[nodiscard]] const wchar_t* AsWide() const noexcept { return nullptr; }

    const char* data;
};
