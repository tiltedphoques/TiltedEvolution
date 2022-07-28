#pragma once

template<class... T>
struct OwnerView
{
    using TView = entt::basic_view<entt::entity, entt::get_t<OwnerComponent, T...>, entt::exclude_t<>>;

    struct iterator
    {
        iterator(Player* apPlayer, typename TView::iterator aItor, typename TView::iterator aEnd, const TView& aView) 
            : m_pPlayer{apPlayer}
            , m_itor{aItor}
            , m_end{aEnd}
            , m_view{aView}
        {
        }

        iterator& operator++()
        {
            do
            {
                m_itor++;
            } while (m_itor != m_end && m_view.template get<OwnerComponent>(*m_itor).GetOwner() != m_pPlayer);

            return *this;
        }

        bool operator!=(const iterator& acRhs) const
        {
            return m_itor != acRhs.m_itor;
        }

        bool operator==(const iterator& acRhs) const
        {
            return m_itor == acRhs.m_itor;
        }

        decltype(auto) operator*()
        {
            return m_itor.operator*();
        }

        [[nodiscard]] decltype(auto) operator->() const
        {
            return m_itor.operator->();
        }

        [[nodiscard]] decltype(auto) operator*() const
        {
            return m_itor.operator*();
        }

      private:
        Player* m_pPlayer;
        typename TView::iterator m_itor;
        typename TView::iterator m_end;
        const TView& m_view;
    };

    decltype(auto) find(entt::entity aEntity) const;
    decltype(auto) begin() const;
    decltype(auto) end() const;

    template<class... Components> 
    decltype(auto) get(entt::entity aEntity);

    template<class... Components> 
    decltype(auto) get(entt::entity aEntity) const;

    OwnerView(entt::registry& aRegistry, Player* apPlayer);

private:

    TView m_view;
    Player* m_pPlayer;
};


template <class... T> 
OwnerView<T...>::OwnerView(entt::registry& aRegistry, Player* apPlayer) 
    : m_view(aRegistry.view<OwnerComponent, T...>())
    , m_pPlayer(apPlayer)
{
}

template <class... T> 
decltype(auto) OwnerView<T...>::find(entt::entity aEntity) const
{
    auto it = m_view.find(aEntity);

    if (it == m_view.end())
        return end();

    if (m_view.template get<OwnerComponent>(*it).GetOwner() == m_pPlayer)
        return iterator(m_pPlayer, it, std::end(m_view), m_view);

    return end();
}

template <class... T> 
decltype(auto) OwnerView<T...>::begin() const
{
    return iterator(m_pPlayer, std::begin(m_view), std::end(m_view), m_view);
}

template <class... T> 
decltype(auto) OwnerView<T...>::end() const
{
    return iterator(m_pPlayer, std::end(m_view), std::end(m_view), m_view);
}

template <class... T> 
template <class... Components> 
decltype(auto) OwnerView<T...>::get(entt::entity aEntity)
{
    return m_view.template get<Components...>(aEntity);
}

template <class... T> 
template <class... Components> 
decltype(auto) OwnerView<T...>::get(entt::entity aEntity) const
{
    return m_view.template get<Components...>(aEntity);
}
