#include <Services/MagicService.h>

#include <Components.h>
#include <GameServer.h>
#include <World.h>
#include <Game/OwnerView.h>

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

    // TODO: this stuff is unfortunately kinda repetitive, maybe integrate it into PlayerManager somehow
    const auto cCasterEntity = static_cast<entt::entity>(message.CasterId);
    const auto* casterCellIdComp = m_world.try_get<CellIdComponent>(cCasterEntity);
    const auto* casterOwnerComp = m_world.try_get<OwnerComponent>(cCasterEntity);

    if (!casterCellIdComp || !casterOwnerComp)
    {
        spdlog::warn("Caster entity not found for server id {:X}", message.CasterId);
        return;
    }

    if (casterCellIdComp->WorldSpaceId == GameId{})
    {
        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (casterOwnerComp->GetOwner() == pPlayer || casterCellIdComp->Cell != pPlayer->GetCellComponent().Cell)
                continue;

            pPlayer->Send(notify);
        }
    }
    else
    {
        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (casterOwnerComp->GetOwner() == pPlayer)
                continue;

            if (pPlayer->GetCellComponent().WorldSpaceId == casterCellIdComp->WorldSpaceId && 
              GridCellCoords::IsCellInGridCell(pPlayer->GetCellComponent().CenterCoords, casterCellIdComp->CenterCoords))
                pPlayer->Send(notify);
        }
    }
}

void MagicService::OnInterruptCastRequest(const PacketEvent<InterruptCastRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifyInterruptCast notify;
    notify.CasterId = message.CasterId;

    const auto cCasterEntity = static_cast<entt::entity>(message.CasterId);
    const auto* casterCellIdComp = m_world.try_get<CellIdComponent>(cCasterEntity);
    const auto* casterOwnerComp = m_world.try_get<OwnerComponent>(cCasterEntity);

    if (!casterCellIdComp || !casterOwnerComp)
    {
        spdlog::warn("Caster entity not found for server id {:X}", message.CasterId);
        return;
    }

    if (casterCellIdComp->WorldSpaceId == GameId{})
    {
        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (casterOwnerComp->GetOwner() == pPlayer || casterCellIdComp->Cell != pPlayer->GetCellComponent().Cell)
                continue;

            pPlayer->Send(notify);
        }
    }
    else
    {
        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (casterOwnerComp->GetOwner() == pPlayer)
                continue;

            if (pPlayer->GetCellComponent().WorldSpaceId == casterCellIdComp->WorldSpaceId && 
              GridCellCoords::IsCellInGridCell(pPlayer->GetCellComponent().CenterCoords, casterCellIdComp->CenterCoords))
                pPlayer->Send(notify);
        }
    }
}

void MagicService::OnAddTargetRequest(const PacketEvent<AddTargetRequest>& acMessage) const noexcept
{
    auto& message = acMessage.Packet;

    NotifyAddTarget notify;
    notify.TargetId = message.TargetId;
    notify.SpellId = message.SpellId;

    const auto cTargetEntity = static_cast<entt::entity>(message.TargetId);
    const auto* targetCellIdComp = m_world.try_get<CellIdComponent>(cTargetEntity);
    const auto* targetOwnerComp = m_world.try_get<OwnerComponent>(cTargetEntity);

    if (!targetCellIdComp || !targetOwnerComp)
    {
        spdlog::warn("Target entity not found for server id {:X}", message.TargetId);
        return;
    }

    if (targetCellIdComp->WorldSpaceId == GameId{})
    {
        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (targetOwnerComp->GetOwner() == pPlayer || targetCellIdComp->Cell != pPlayer->GetCellComponent().Cell)
                continue;

            pPlayer->Send(notify);
        }
    }
    else
    {
        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (targetOwnerComp->GetOwner() == pPlayer)
                continue;

            if (pPlayer->GetCellComponent().WorldSpaceId == targetCellIdComp->WorldSpaceId && 
              GridCellCoords::IsCellInGridCell(pPlayer->GetCellComponent().CenterCoords, targetCellIdComp->CenterCoords))
                pPlayer->Send(notify);
        }
    }
}

