#include <Services/MagicService.h>

#include <GameServer.h>
#include <World.h>

#include <Messages/SpellCastRequest.h>
#include <Messages/InterruptCastRequest.h>
#include <Messages/AddTargetRequest.h>

#include <Messages/NotifySpellCast.h>
#include <Messages/NotifyInterruptCast.h>
#include <Messages/NotifyAddTarget.h>

MagicService::MagicService(World& aWorld, entt::dispatcher& aDispatcher) noexcept 
    : m_world(aWorld)
{
    m_spellCastConnection = aDispatcher.sink<PacketEvent<SpellCastRequest>>().connect<&MagicService::OnSpellCastRequest>(this);
    m_interruptCastConnection = aDispatcher.sink<PacketEvent<InterruptCastRequest>>().connect<&MagicService::OnInterruptCastRequest>(this);
    m_addTargetConnection = aDispatcher.sink<PacketEvent<AddTargetRequest>>().connect<&MagicService::OnAddTargetRequest>(this);
}

void MagicService::OnSpellCastRequest(const PacketEvent<SpellCastRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifySpellCast notify;
    notify.CasterId = message.CasterId;
    notify.SpellFormId = message.SpellFormId;
    notify.CastingSource = message.CastingSource;
    notify.IsDualCasting = message.IsDualCasting;
    notify.DesiredTarget = message.DesiredTarget;

    const auto entity = static_cast<entt::entity>(message.CasterId);
    GameServer::Get()->SendToPlayersInRange(notify, entity, acMessage.GetSender());
}

void MagicService::OnInterruptCastRequest(const PacketEvent<InterruptCastRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifyInterruptCast notify;
    notify.CasterId = message.CasterId;
    notify.CastingSource = message.CastingSource;

    const auto entity = static_cast<entt::entity>(message.CasterId);
    GameServer::Get()->SendToPlayersInRange(notify, entity, acMessage.GetSender());
}

void MagicService::OnAddTargetRequest(const PacketEvent<AddTargetRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifyAddTarget notify;
    notify.TargetId = message.TargetId;
    notify.SpellId = message.SpellId;
    notify.EffectId = message.EffectId;
    notify.Magnitude = message.Magnitude;

    const auto entity = static_cast<entt::entity>(message.TargetId);
    GameServer::Get()->SendToPlayersInRange(notify, entity, acMessage.GetSender());
}

