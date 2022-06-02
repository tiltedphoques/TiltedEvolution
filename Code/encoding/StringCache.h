#pragma once

#include <Messages/StringCacheUpdate.h>

struct StringCache
{
    TP_NOCOPYMOVE(StringCache);

    [[nodiscard]] bool Contains(const TiltedPhoques::String&) const noexcept;

    [[nodiscard]] std::optional<uint32_t> operator[](const TiltedPhoques::String&) const noexcept;
    [[nodiscard]] std::optional<const TiltedPhoques::String> operator[](uint32_t) const noexcept;
    uint32_t Add(const TiltedPhoques::String&) noexcept;
    [[nodiscard]] void AddWanted(const TiltedPhoques::String&) noexcept;
    [[nodiscard]] size_t Size() const noexcept;
    [[nodiscard]] StringCacheUpdate Serialize(uint32_t& aStartId) const noexcept;
    [[nodiscard]] void Deserialize(const StringCacheUpdate& aMessage) noexcept;
    void Clear() noexcept;
    bool ProcessDirty() noexcept;
    void ClearDirty() noexcept;

    static StringCache& Get() noexcept;

private:

    TiltedPhoques::Vector<TiltedPhoques::String> m_idToString;
    mutable TiltedPhoques::Set<TiltedPhoques::String> m_wantedStrings;
    TiltedPhoques::Map<TiltedPhoques::String, uint32_t> m_stringToId;

    StringCache();
};
