#include <StringCache.h>
#include <iostream>

bool StringCache::Contains(const TiltedPhoques::String& acValue) const noexcept
{
    return m_stringToId.contains(acValue);
}

std::optional<uint32_t> StringCache::operator[](const TiltedPhoques::String& acValue) const noexcept
{
    if (const auto itor = m_stringToId.find(acValue); itor != std::end(m_stringToId))
        return itor->second;

    return std::nullopt;
}

std::optional<const TiltedPhoques::String> StringCache::operator[](uint32_t aValue) const noexcept
{
    if (aValue < m_idToString.size())
        return m_idToString[aValue];

    return std::nullopt;
}

uint32_t StringCache::Add(const TiltedPhoques::String& acValue) noexcept
{
    if (auto id = this->operator[](acValue))
        return *id;

    const auto allocatedId = m_idToString.size();
    m_stringToId[acValue] = allocatedId & 0xFFFFFFFF;
    m_idToString.push_back(acValue);

    return allocatedId;
}

void StringCache::AddWanted(const TiltedPhoques::String& acValue) noexcept
{
    m_wantedStrings.insert(acValue);
}

size_t StringCache::Size() const noexcept
{
    return m_idToString.size();
}

StringCacheUpdate StringCache::Serialize(uint32_t& aStartId) const noexcept
{
    StringCacheUpdate update;
    update.StartId = aStartId;

    if (aStartId < m_idToString.size())
    {
        auto itor = std::begin(m_idToString);
        std::advance(itor, aStartId);

        update.Values.insert(std::begin(update.Values), itor, std::end(m_idToString));

        aStartId = m_idToString.size();
    }

    return update;
}

void StringCache::Deserialize(const StringCacheUpdate& aMessage) noexcept
{
    // We should only receive contiguous updates
    assert(aMessage.StartId == m_idToString.size());

    for (auto& value : aMessage.Values)
    {
        const auto expectedId = m_idToString.size();
        auto id = Add(value);

        assert(id == expectedId);
    }
}

void StringCache::Clear() noexcept
{
    m_idToString.clear();
    m_wantedStrings.clear();
    m_stringToId.clear();
}

bool StringCache::ProcessDirty() noexcept
{
    if (m_wantedStrings.empty())
        return false;

    for (auto& s : m_wantedStrings)
        Add(s);

    ClearDirty();

    return true;
}

void StringCache::ClearDirty() noexcept
{
    m_wantedStrings.clear();
}

StringCache& StringCache::Get() noexcept
{
    TiltedPhoques::ScopedAllocator _{TiltedPhoques::Allocator::GetDefault()};
    {
        static StringCache s_instance;
        return s_instance;
    }
}

StringCache::StringCache()
{
    
}
