#include <TiltedOnlinePCH.h>
#include <Misc/BSFixedString.h>

#include <utility>

namespace
{
struct StringEntry
{
    StringEntry* pLeft;
    uint16_t flags;
    uint16_t crc;
    union
    {
        uint32_t length;
        StringEntry* pRight;
    };
};
static_assert(sizeof(StringEntry) == 0x18);

void Acquire(const char* apEntry) noexcept
{
    if (!apEntry)
        return;
    auto* pFlags = reinterpret_cast<volatile short*>(const_cast<char*>(apEntry) + 8);
    short expected = *pFlags;
    while ((expected & 0x3FFF) < 0x3FFF)
    {
        const short previous = _InterlockedCompareExchange16(pFlags, static_cast<short>(expected + 1), expected);
        if (previous == expected)
            return;
        expected = previous;
    }
}

const StringEntry* Leaf(const char* apEntry) noexcept
{
    auto* pEntry = reinterpret_cast<const StringEntry*>(apEntry);
    while (pEntry && (pEntry->flags & 0x4000))
        pEntry = pEntry->pRight;
    return pEntry;
}
}

BSFixedString::BSFixedString() : data(nullptr)
{
}

BSFixedString::BSFixedString(const char* acpData, bool aCaseSensitive) : data(nullptr)
{
    Set(acpData, aCaseSensitive);
}

BSFixedString::BSFixedString(const BSFixedString& acOther) : data(acOther.data)
{
    Acquire(data);
}

BSFixedString::BSFixedString(BSFixedString&& aOther) noexcept : data(std::exchange(aOther.data, nullptr))
{
}

BSFixedString& BSFixedString::operator=(const BSFixedString& acOther)
{
    if (this != &acOther)
    {
        Acquire(acOther.data);
        Release();
        data = acOther.data;
    }
    return *this;
}

BSFixedString& BSFixedString::operator=(BSFixedString&& aOther) noexcept
{
    if (this != &aOther)
    {
        Release();
        data = std::exchange(aOther.data, nullptr);
    }
    return *this;
}

BSFixedString::~BSFixedString()
{
    Release();
}

void BSFixedString::Set(const char* acpData, bool aCaseSensitive)
{
    using TGetEntry = void(const char*&, const char*, bool);
    static VersionDbPtr<TGetEntry> s_getEntry(2268729);
    const char* pNewEntry = nullptr;
    if (acpData)
        s_getEntry.Get()(pNewEntry, acpData, aCaseSensitive);
    Release();
    data = pNewEntry;
}

void BSFixedString::Release() noexcept
{
    using TRelease = void(const char*&);
    static VersionDbPtr<TRelease> s_release(2268720);
    if (data)
        s_release.Get()(data);
    data = nullptr;
}

bool BSFixedString::IsAscii() const noexcept
{
    const auto* pEntry = Leaf(data);
    return !pEntry || !(pEntry->flags & 0x8000);
}

const char* BSFixedString::AsAscii() const noexcept
{
    const auto* pEntry = Leaf(data);
    return pEntry && !(pEntry->flags & 0x8000) ? reinterpret_cast<const char*>(pEntry + 1) : nullptr;
}

const wchar_t* BSFixedString::AsWide() const noexcept
{
    const auto* pEntry = Leaf(data);
    return pEntry && (pEntry->flags & 0x8000) ? reinterpret_cast<const wchar_t*>(pEntry + 1) : nullptr;
}
