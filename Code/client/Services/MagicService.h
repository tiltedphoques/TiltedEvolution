#pragma once

struct World;
struct TransportService;

struct SpellCastEvent;
struct InterruptCastEvent;
struct AddTargetEvent;

struct NotifySpellCast;
struct NotifyInterruptCast;
struct NotifyAddTarget;

struct MagicService
{
    MagicService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~MagicService() noexcept = default;

    TP_NOCOPYMOVE(MagicService);

protected:

    void OnSpellCastEvent(const SpellCastEvent& acSpellCastEvent) const noexcept;
    void OnNotifySpellCast(const NotifySpellCast& acMessage) const noexcept;
    void OnInterruptCastEvent(const InterruptCastEvent& acEvent) const noexcept;
    void OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept;
    void OnAddTargetEvent(const AddTargetEvent& acEvent) const noexcept;
    void OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept;

private:

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    entt::scoped_connection m_spellCastEventConnection;
    entt::scoped_connection m_notifySpellCastConnection;
    entt::scoped_connection m_interruptCastEventConnection;
    entt::scoped_connection m_notifyInterruptCastConnection;
    entt::scoped_connection m_addTargetEventConnection;
    entt::scoped_connection m_notifyAddTargetConnection;
};
