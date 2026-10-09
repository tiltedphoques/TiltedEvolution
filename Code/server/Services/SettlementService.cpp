#include <Services/SettlementService.h>

#include <World.h>
#include <Components.h>
#include <Messages/SettlementRequest.h>
#include <Messages/NotifySettlement.h>
#include <Events/UpdateEvent.h>
#include <chrono>
#include <limits>

SettlementService::SettlementService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
    , m_nextSessionId(std::chrono::system_clock::now().time_since_epoch().count())
{
    m_requestConnection = aDispatcher.sink<PacketEvent<SettlementRequest>>().connect<&SettlementService::OnRequest>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&SettlementService::OnUpdate>(this);
}

void SettlementService::OnRequest(const PacketEvent<SettlementRequest>& acEvent) noexcept
{
    const auto& request = acEvent.Packet;
    auto* pPlayer = acEvent.pPlayer;
    auto* pParty = m_world.GetPartyService().GetPlayerParty(pPlayer);
    if (!pParty)
        return;
    auto& session = m_sessions[*pPlayer->GetParty().JoinedPartyId];
    if (!session.Id || session.LeaderPlayerId != pParty->LeaderPlayerId)
    {
        session = {};
        session.Id = ++m_nextSessionId;
        session.LeaderPlayerId = pParty->LeaderPlayerId;
        NotifySettlement reset;
        reset.SessionId = session.Id;
        reset.LeaderPlayerId = session.LeaderPlayerId;
        reset.Accepted = true;
        for (auto* pMember : pParty->Members)
            pMember->Send(reset);
    }
    NotifySettlement notify;
    notify.SessionId = session.Id;
    notify.LeaderPlayerId = session.LeaderPlayerId;
    if (request.Operation == SettlementRequest::Action::Snapshot)
    {
        notify.Accepted = true;
        pPlayer->Send(notify);
        for (const auto& [id, record] : session.Objects)
        {
            notify.Data = record.Data;
            pPlayer->Send(notify);
        }
        return;
    }
    if (request.Operation != SettlementRequest::Action::Update || !request.RequestId || !request.Data.IsValid() || !pPlayer->GetCharacter() || request.SessionId != session.Id)
        return;

    notify.SenderId = pPlayer->GetId();
    notify.RequestId = request.RequestId;
    notify.Data = request.Data;
    const auto& cell = pPlayer->GetCellComponent();
    const auto coords = GridCellCoords::CalculateGridCellCoords(request.Data.Position.x, request.Data.Position.y);
    const bool inRange = cell.WorldSpaceId == request.Data.WorldSpace &&
                         (cell.IsInInteriorCell() ? cell.Cell == request.Data.Cell : GridCellCoords::IsCellInGridCell(coords, cell.CenterCoords, false));
    if (!inRange)
    {
        notify.Data.Id = 0;
        notify.Data.Removed = true;
        pPlayer->Send(notify);
        return;
    }

    const bool leader = pPlayer->GetId() == session.LeaderPlayerId;
    const uint64_t origin = (uint64_t(pPlayer->GetId()) << 32) | request.RequestId;
    auto it = session.Objects.find(request.Data.Id);
    if (!request.Data.Id)
    {
        it = std::find_if(session.Objects.begin(), session.Objects.end(), [&](const auto& aPair)
        {
            const auto& record = aPair.second;
            return request.Data.Reference ? record.Data.Workshop == request.Data.Workshop && record.Data.Reference == request.Data.Reference
                                          : (leader && request.Data.LeaderReference ? record.Data.LeaderReference == request.Data.LeaderReference
                                                                                    : record.Origin == origin);
        });
    }
    if (it == session.Objects.end())
    {
        if (request.Data.Id || request.Data.Revision || (!leader && request.Data.LeaderReference) || session.Objects.size() >= 4096)
        {
            notify.Data.Id = 0;
            notify.Data.Removed = true;
            pPlayer->Send(notify);
            return;
        }
        notify.Data.Id = session.NextId++;
        notify.Data.Revision = 1;
        it = session.Objects.emplace(notify.Data.Id, Record{notify.Data, origin}).first;
    }
    else
    {
        const auto& current = it->second.Data;
        if (request.Data.Revision != current.Revision || current.Revision == std::numeric_limits<uint32_t>::max() || current.Removed ||
            request.Data.Workshop != current.Workshop || request.Data.Reference != current.Reference ||
            request.Data.Base != current.Base || request.Data.WorldSpace != current.WorldSpace ||
            (request.Data.LeaderReference != current.LeaderReference && (!leader || current.LeaderReference)))
        {
            notify.Data = current;
            pPlayer->Send(notify);
            return;
        }
        notify.Data.Id = current.Id;
        notify.Data.Revision = current.Revision + 1;
        it->second.Data = notify.Data;
    }
    notify.Accepted = true;
    for (auto* pOther : pParty->Members)
        pOther->Send(notify);
    spdlog::info("Settlement object {} revision {} workshop {:X} removed {}", notify.Data.Id, notify.Data.Revision,
                 notify.Data.Workshop.LogFormat(), notify.Data.Removed);
}

void SettlementService::OnUpdate(const UpdateEvent&) noexcept
{
    for (auto it = m_sessions.begin(); it != m_sessions.end();)
    {
        const auto* pParty = m_world.GetPartyService().GetById(it->first);
        if (!pParty || pParty->LeaderPlayerId != it->second.LeaderPlayerId)
            it = m_sessions.erase(it);
        else
            ++it;
    }
}
