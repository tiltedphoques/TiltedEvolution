#include <TiltedOnlinePCH.h>

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>

#include <Services/StringCacheService.h>

StringCacheService::StringCacheService(entt::dispatcher& aDispatcher) noexcept
{
    m_connectedConnection = aDispatcher.sink<ConnectedEvent>().connect<&StringCacheService::HandleConnected>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&StringCacheService::HandleDisconnected>(this);
//    m_stringCacheContentConnection = aDispatcher.sink<TiltedMessages::StringCacheContent>().connect<&StringCacheService::HandleStringCacheContent>(this);
}

const String& StringCacheService::Get(const uint32_t aIndex) const noexcept
{
    static const String s_dummy;

    const auto itor = m_cache.find(aIndex);
    if (itor != std::end(m_cache))
        return itor->second;

    return s_dummy;
}

void StringCacheService::HandleConnected(const ConnectedEvent& acEvent) noexcept
{
}

void StringCacheService::HandleDisconnected(const DisconnectedEvent& acEvent) noexcept
{
    m_cache.clear();
}

/*
void StringCacheService::HandleStringCacheContent(const TiltedMessages::StringCacheContent& acMessage) noexcept
{
    for (auto& content : acMessage.data())
    {
        m_cache.emplace(content.first, String(content.second.c_str(), content.second.size()));
    }
}
*/
