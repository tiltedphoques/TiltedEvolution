#pragma once

namespace TiltedMessages
{
    class StringCacheContent;
}


struct ConnectedEvent;
struct DisconnectedEvent;
struct StringCacheService
{
    StringCacheService(entt::dispatcher& aDispatcher) noexcept;
    ~StringCacheService() noexcept = default;

    TP_NOCOPYMOVE(StringCacheService);

    const String& Get(uint32_t aIndex) const noexcept;

    void HandleConnected(const ConnectedEvent&) noexcept;
    void HandleDisconnected(const DisconnectedEvent&) noexcept;

    //void HandleStringCacheContent(const TiltedMessages::StringCacheContent&) noexcept;

private:

    Map<uint32_t, String> m_cache;

    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_stringCacheContentConnection;
    
};
