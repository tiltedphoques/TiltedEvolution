#pragma once

struct Player;

struct PlayerManager
{
    using TMap = TiltedPhoques::Map<ConnectionId_t, UniquePtr<Player>>;

    struct Iterator
    {
        Iterator(TMap::iterator aItor) : m_itor(aItor){}
        Iterator operator++() { m_itor++; return *this; }
        bool operator!=(const Iterator& acRhs) const { return m_itor != acRhs.m_itor; }
        const Player* operator*() const { return m_itor->second.get(); }
        Player* operator*() { return m_itor.value().get(); }

    private:
        TMap::iterator m_itor;
    };

    PlayerManager();
    ~PlayerManager() noexcept = default;

    TP_NOCOPYMOVE(PlayerManager);

    Iterator begin(){return Iterator(std::begin(m_players));}
    Iterator end(){return Iterator(std::end(m_players));}

    Player* Create(ConnectionId_t aConnectionId) noexcept;
    void Remove(Player* apPlayer) noexcept;
    Player* GetByConnectionId(ConnectionId_t aConnectionId) noexcept;
    Player const* GetByConnectionId(ConnectionId_t aConnectionId) const noexcept;

    Player* GetById(uint32_t aId) noexcept;
    Player const* GetById(uint32_t aId) const noexcept;

    uint32_t Count() const noexcept;

    template<class T>
    void ForEach(const T& acFunctor) noexcept
    {
        auto itor = std::begin(m_players);
        auto end = std::end(m_players);
        for (; itor != end; ++itor)
        {
            acFunctor(itor.value().get());
        }
    }

private:

    TMap m_players;
};
