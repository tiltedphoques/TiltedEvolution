#pragma once

struct BSFixedString
{
    BSFixedString();
    BSFixedString(const char* acpData, bool aCaseSensitive = false);
    BSFixedString(const BSFixedString& acOther);
    BSFixedString(BSFixedString&& aOther) noexcept;
    BSFixedString& operator=(const BSFixedString& acOther);
    BSFixedString& operator=(BSFixedString&& aOther) noexcept;
    ~BSFixedString();

    void Set(const char* acpData, bool aCaseSensitive = false);
    void Release() noexcept;

    operator const char*() const noexcept { return AsAscii(); }

    [[nodiscard]] bool IsAscii() const noexcept;
    [[nodiscard]] const char* AsAscii() const noexcept;
    [[nodiscard]] const wchar_t* AsWide() const noexcept;

    const char* data;
};

static_assert(sizeof(BSFixedString) == 0x8);
