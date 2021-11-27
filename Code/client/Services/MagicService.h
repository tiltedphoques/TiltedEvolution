#pragma once

struct World;
struct TransportService;

struct UpdateEvent;
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

    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnSpellCastEvent(const SpellCastEvent& acSpellCastEvent) const noexcept;
    void OnNotifySpellCast(const NotifySpellCast& acMessage) const noexcept;
    void OnInterruptCastEvent(const InterruptCastEvent& acEvent) const noexcept;
    void OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept;
    void OnAddTargetEvent(const AddTargetEvent& acEvent) noexcept;
    void OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept;

private:

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    Map<uint32_t, uint32_t> m_queuedEffects;

    entt::scoped_connection m_spellCastEventConnection;
    entt::scoped_connection m_notifySpellCastConnection;
    entt::scoped_connection m_interruptCastEventConnection;
    entt::scoped_connection m_notifyInterruptCastConnection;
    entt::scoped_connection m_addTargetEventConnection;
    entt::scoped_connection m_notifyAddTargetConnection;
};
