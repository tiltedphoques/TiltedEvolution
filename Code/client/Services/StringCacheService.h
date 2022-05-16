#pragma once

struct ConnectedEvent;
struct DisconnectedEvent;
struct StringCacheUpdate;

/**
* @brief Caches strings to reduce data usage when sending string data between clients.
* 
* This service is currently not in use.
*/
struct StringCacheService
{
    StringCacheService(entt::dispatcher& aDispatcher) noexcept;
    ~StringCacheService() noexcept = default;

    TP_NOCOPYMOVE(StringCacheService);

    void HandleConnected(const ConnectedEvent&) noexcept;
    void HandleDisconnected(const DisconnectedEvent&) noexcept;

    void HandleStringCacheUpdate(const StringCacheUpdate&) noexcept;

private:

    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_stringCacheUpdateConnection;
    
};
