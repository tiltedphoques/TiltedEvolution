#pragma once

#include <Events/PacketEvent.h>

struct World;
struct SpellCastRequest;
struct InterruptCastRequest;
struct AddTargetRequest;

struct MagicService
{
    MagicService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~MagicService() noexcept = default;

    TP_NOCOPYMOVE(MagicService);

protected:

    void OnSpellCastRequest(const PacketEvent<SpellCastRequest>& acMessage) const noexcept;
    void OnInterruptCastRequest(const PacketEvent<InterruptCastRequest>& acMessage) const noexcept;
    void OnAddTargetRequest(const PacketEvent<AddTargetRequest>& acMessage) const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_spellCastConnection;
    entt::scoped_connection m_interruptCastConnection;
    entt::scoped_connection m_addTargetConnection;
};
