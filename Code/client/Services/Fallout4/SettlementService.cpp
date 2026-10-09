#if defined(TP_FALLOUT4)
#include <Services/SettlementService.h>
#include <Services/TransportService.h>
#include <World.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/UpdateEvent.h>
#include <Events/CellChangeEvent.h>
#include <Events/PartyJoinedEvent.h>
#include <Events/PartyLeftEvent.h>
#include <Messages/SettlementRequest.h>
#include <Messages/NotifySettlement.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>
#include <Forms/TESBoundObject.h>
#include <Games/TES.h>

namespace
{
const GlobalEventSource<Workshop::ItemPlacedEvent> s_placed{2194926, 2194927};
const GlobalEventSource<Workshop::ItemMovedEvent> s_moved{2194932, 2194933};
const GlobalEventSource<Workshop::ItemDestroyedEvent> s_destroyed{2194930, 2194931};

bool IsWorkshopObject(const TESForm* apBase)
{
    if (!apBase)
        return false;
    switch (static_cast<uint8_t>(apBase->formType))
    {
    case 0x1B: // Activator
    case 0x1F: // Container
    case 0x20: // Door
    case 0x22: // Light
    case 0x24: // Static
    case 0x25: // Static collection
    case 0x26: // Movable static
    case 0x28: // Tree
    case 0x29: // Flora
    case 0x2A: // Furniture
    case 0x37: // Terminal
        return true;
    default:
        return false;
    }
}
}

SettlementService::SettlementService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport)
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_connectedConnection = aDispatcher.sink<ConnectedEvent>().connect<&SettlementService::OnConnected>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&SettlementService::OnDisconnected>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&SettlementService::OnUpdate>(this);
    m_cellChangeConnection = aDispatcher.sink<CellChangeEvent>().connect<&SettlementService::OnCellChange>(this);
    m_notifyConnection = aDispatcher.sink<NotifySettlement>().connect<&SettlementService::OnNotify>(this);
    m_partyJoinedConnection = aDispatcher.sink<PartyJoinedEvent>().connect<&SettlementService::OnPartyJoined>(this);
    m_partyLeftConnection = aDispatcher.sink<PartyLeftEvent>().connect<&SettlementService::OnPartyLeft>(this);
    s_placed.RegisterSink(this);
    s_moved.RegisterSink(this);
    s_destroyed.RegisterSink(this);
}

SettlementService::~SettlementService()
{
    s_placed.UnRegisterSink(this);
    s_moved.UnRegisterSink(this);
    s_destroyed.UnRegisterSink(this);
}

BSTEventResult SettlementService::OnEvent(const Workshop::ItemPlacedEvent* apEvent, const EventDispatcher<Workshop::ItemPlacedEvent>*)
{
    if (apEvent)
        Capture(apEvent->Workshop, apEvent->Item, false);
    return BSTEventResult::kOk;
}

BSTEventResult SettlementService::OnEvent(const Workshop::ItemMovedEvent* apEvent, const EventDispatcher<Workshop::ItemMovedEvent>*)
{
    if (apEvent)
        Capture(apEvent->Workshop, apEvent->Item, false);
    return BSTEventResult::kOk;
}

BSTEventResult SettlementService::OnEvent(const Workshop::ItemDestroyedEvent* apEvent, const EventDispatcher<Workshop::ItemDestroyedEvent>*)
{
    if (apEvent)
        Capture(apEvent->Workshop, apEvent->Item, true);
    return BSTEventResult::kOk;
}

bool SettlementService::ReadObject(TESObjectREFR* apWorkshop, TESObjectREFR* apItem, bool aRemoved, SettlementObject& aData) noexcept
{
    if (!apWorkshop || !apItem || apWorkshop->IsTemporary() || !IsWorkshopObject(apItem->baseForm))
        return false;
    auto* pCell = apItem->GetParentCellEx();
    if (!pCell)
        pCell = apWorkshop->GetParentCellEx();
    if (!pCell)
        return false;
    auto& data = aData;
    auto& mods = m_world.GetModSystem();
    if (!mods.GetServerModId(apWorkshop->formID, data.Workshop) || !mods.GetServerModId(apItem->baseForm->formID, data.Base) ||
        !mods.GetServerModId(pCell->formID, data.Cell))
        return false;
    if (!apItem->IsTemporary() && !mods.GetServerModId(apItem->formID, data.Reference))
        return false;
    if (auto* pWorld = pCell->worldspace)
        if (!mods.GetServerModId(pWorld->formID, data.WorldSpace))
            return false;
    data.Position = {apItem->position.x, apItem->position.y, apItem->position.z};
    data.Rotation = {apItem->rotation.x, apItem->rotation.y, apItem->rotation.z};
    data.Scale = apItem->scale ? apItem->scale : 100;
    data.Removed = aRemoved;
    if (m_isLeader && apItem->IsTemporary())
        data.LeaderReference = apItem->formID;
    return data.IsValid();
}

void SettlementService::Capture(TESObjectREFR* apWorkshop, TESObjectREFR* apItem, bool aRemoved) noexcept
{
    if (m_applying)
        return;
    if (aRemoved && apWorkshop && !apWorkshop->IsTemporary() && apItem && !apItem->IsTemporary() &&
        IsWorkshopObject(apItem->baseForm) && (!m_transport.IsOnline() || m_isLeader))
    {
        const auto localId = apItem->formID;
        const auto workshopId = apWorkshop->formID;
        m_world.GetRunner().Queue([localId, workshopId]()
        {
            auto* pRef = Cast<TESObjectREFR>(TESForm::GetById(localId));
            auto* pWorkshop = Cast<TESObjectREFR>(TESForm::GetById(workshopId));
            if (pRef && pWorkshop)
            {
                using SetWorkshop = void(TESObjectREFR*, bool, TESObjectREFR*);
                static VersionDbPtr<SetWorkshop> setWorkshop(2194972);
                setWorkshop(pRef, true, pWorkshop);
            }
        });
    }
    if (!m_transport.IsOnline() || !m_sessionId || !m_world.GetPartyService().IsInParty())
        return;
    SettlementObject data{};
    if (!ReadObject(apWorkshop, apItem, aRemoved, data))
        return;
    if (!m_isLeader && apItem->IsTemporary())
        apItem->SetSkipSaveFlag(true);
    const auto localId = apItem->formID;
    const auto sessionId = m_sessionId;
    m_world.GetRunner().Queue([this, data, localId, sessionId]()
    {
        if (!m_transport.IsOnline() || sessionId != m_sessionId)
            return;
        auto& pending = m_pending[localId];
        pending.Data = data;
        spdlog::info("Settlement captured {:X} removed {}", localId, data.Removed);
    });
}

void SettlementService::ImportLoaded() noexcept
{
    auto* pTes = TES::Get();
    if (!pTes)
        return;
    using GetWorkshop = TESObjectREFR*(TESObjectREFR*);
    static VersionDbPtr<GetWorkshop> getWorkshop(2194973);
    const auto scan = [&](TESObjectCELL* apCell)
    {
        if (!apCell || !apCell->IsAttached())
            return;
        for (auto* pRef : apCell->objectList)
        {
            if (!pRef || !pRef->extraData || !IsWorkshopObject(pRef->baseForm) || (pRef->IsTemporary() && (pRef->IsDeleted() || pRef->IsDisabled())) || m_pending.contains(pRef->formID))
                continue;
            if (std::any_of(m_objects.begin(), m_objects.end(), [&](const auto& aPair)
                { return aPair.second.LocalId == pRef->formID || (pRef->IsTemporary() && aPair.second.Data.LeaderReference == pRef->formID); }))
                continue;
            auto* pWorkshop = getWorkshop(pRef);
            SettlementObject data{};
            if (ReadObject(pWorkshop, pRef, pRef->IsDisabled() || pRef->IsDeleted(), data))
                m_pending[pRef->formID].Data = data;
        }
    };
    if (pTes->interiorCell)
        scan(pTes->interiorCell);
    else if (auto* pGrid = pTes->cells; pGrid && pGrid->arr)
        for (uint32_t i = 0; i < pGrid->dimension * pGrid->dimension; ++i)
            scan(pGrid->arr[i]);
}

void SettlementService::OnConnected(const ConnectedEvent&) noexcept
{
    m_leaderId.reset();
    m_sessionId = 0;
}

void SettlementService::ClearReplicas() noexcept
{
    m_applying = true;
    if (!m_isLeader)
        for (auto& [localId, pending] : m_pending)
            if (!pending.Data.Reference)
                if (auto* pRef = Cast<TESObjectREFR>(TESForm::GetById(localId)))
                    pRef->Delete();
    for (auto& [id, record] : m_objects)
    {
        if (record.Spawned && !m_isLeader)
        {
            if (auto* pRef = Cast<TESObjectREFR>(TESForm::GetById(record.LocalId)))
                pRef->Delete();
            record.LocalId = 0;
            record.Spawned = false;
        }
        record.NeedsApply = true;
    }
    m_applying = false;
}

void SettlementService::Reset() noexcept
{
    ClearReplicas();
    m_pending.clear();
    m_objects.clear();
    m_leaderId.reset();
    m_sessionId = 0;
}

void SettlementService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    Reset();
}

void SettlementService::OnPartyJoined(const PartyJoinedEvent&) noexcept
{
    Reset();
}

void SettlementService::OnPartyLeft(const PartyLeftEvent&) noexcept
{
    Reset();
}

void SettlementService::OnNotify(const NotifySettlement& acMessage) noexcept
{
    const auto& party = m_world.GetPartyService();
    if (!acMessage.SessionId || !party.IsInParty() || acMessage.LeaderPlayerId != party.GetLeaderPlayerId())
        return;
    if (m_sessionId != acMessage.SessionId)
    {
        ClearReplicas();
        m_objects.clear();
        m_pending.clear();
        m_isLeader = party.IsLeader();
        m_leaderId = acMessage.LeaderPlayerId;
        m_scanElapsed = 2;
        m_sessionId = acMessage.SessionId;
    }
    const bool own = acMessage.SenderId == m_transport.GetLocalPlayerId() && acMessage.RequestId;
    auto pending = own ? m_pending.find(acMessage.RequestId) : m_pending.end();
    if (!acMessage.Data.Id || !acMessage.Data.IsValid())
    {
        if (pending != m_pending.end())
            m_pending.erase(pending);
        return;
    }
    auto& record = m_objects[acMessage.Data.Id];
    if (record.Data.Revision > acMessage.Data.Revision)
        return;
    record.Data = acMessage.Data;
    record.NeedsApply = true;
    if (own && pending != m_pending.end())
    {
        record.LocalId = acMessage.RequestId;
        if (!m_isLeader && !record.Data.Reference)
        {
            record.Spawned = true;
            if (auto* pRef = TESForm::GetById(record.LocalId))
                pRef->SetSkipSaveFlag(true);
        }
        pending->second.Waiting = false;
        pending->second.Data.Id = record.Data.Id;
        pending->second.Data.Revision = record.Data.Revision;
        pending->second.Data.LeaderReference = record.Data.LeaderReference;
        if (!acMessage.Accepted || pending->second.Data == record.Data)
            m_pending.erase(pending);
        if (acMessage.Accepted)
            record.NeedsApply = false;
    }
}

void SettlementService::OnCellChange(const CellChangeEvent&) noexcept
{
    for (auto& [id, record] : m_objects)
        record.NeedsApply = true;
}

void SettlementService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    if (!m_transport.IsOnline())
        return;
    const auto& party = m_world.GetPartyService();
    if (!party.IsInParty())
    {
        if (m_leaderId)
        {
            ClearReplicas();
            m_objects.clear();
            m_pending.clear();
            m_leaderId.reset();
            m_sessionId = 0;
        }
        return;
    }
    if (!m_leaderId || *m_leaderId != party.GetLeaderPlayerId())
    {
        ClearReplicas();
        m_objects.clear();
        m_pending.clear();
        m_sessionId = 0;
        m_leaderId = party.GetLeaderPlayerId();
        m_isLeader = party.IsLeader();
        m_transport.Send(SettlementRequest{});
    }
    if (!m_sessionId)
        return;
    m_elapsed += acEvent.Delta;
    if (m_elapsed < 0.5)
        return;
    m_elapsed = 0;
    if (m_isLeader)
    {
        m_scanElapsed += 0.5;
        if (m_scanElapsed >= 2)
        {
            m_scanElapsed = 0;
            ImportLoaded();
        }
    }
    for (auto& [localId, pending] : m_pending)
    {
        if (pending.Waiting)
            continue;
        for (const auto& [id, record] : m_objects)
        {
            if (record.LocalId == localId || (pending.Data.Reference && pending.Data.Reference == record.Data.Reference && pending.Data.Workshop == record.Data.Workshop))
            {
                pending.Data.Id = id;
                pending.Data.Revision = record.Data.Revision;
                if (!m_isLeader || record.Data.LeaderReference)
                    pending.Data.LeaderReference = record.Data.LeaderReference;
                break;
            }
        }
        SettlementRequest request;
        request.Operation = SettlementRequest::Action::Update;
        request.SessionId = m_sessionId;
        request.RequestId = localId;
        request.Data = pending.Data;
        pending.Waiting = m_transport.Send(request);
    }
    m_applying = true;
    for (auto& [id, record] : m_objects)
    {
        if (!record.NeedsApply && !record.Data.Removed && !TESForm::GetById(record.LocalId))
            record.NeedsApply = true;
        if (record.NeedsApply)
            record.NeedsApply = !Apply(record);
    }
    m_applying = false;
}

bool SettlementService::Apply(Record& aRecord) noexcept
{
    const auto& data = aRecord.Data;
    auto& mods = m_world.GetModSystem();
    auto* pCell = Cast<TESObjectCELL>(TESForm::GetById(mods.GetGameId(data.Cell)));
    if (!pCell || !pCell->IsAttached())
        return false;
    auto* pRef = Cast<TESObjectREFR>(TESForm::GetById(aRecord.LocalId));
    if (!pRef && m_isLeader && data.LeaderReference)
        pRef = Cast<TESObjectREFR>(TESForm::GetById(data.LeaderReference));
    if (!pRef && data.Reference)
        pRef = Cast<TESObjectREFR>(TESForm::GetById(mods.GetGameId(data.Reference)));
    auto* pBase = Cast<TESBoundObject>(TESForm::GetById(mods.GetGameId(data.Base)));
    if (!IsWorkshopObject(pBase) || (pRef && pRef->baseForm != pBase))
        return false;
    auto* pWorkshop = Cast<TESObjectREFR>(TESForm::GetById(mods.GetGameId(data.Workshop)));
    if (!pWorkshop)
        return false;
    if (data.Removed)
    {
        if (!pRef && data.Reference)
            return false;
        if (pRef)
        {
            if (!data.Reference)
                pRef->Delete();
            else
            {
                if (m_isLeader)
                {
                    using SetWorkshop = void(TESObjectREFR*, bool, TESObjectREFR*);
                    static VersionDbPtr<SetWorkshop> setWorkshop(2194972);
                    setWorkshop(pRef, true, pWorkshop);
                }
                pRef->DisableImpl();
            }
        }
        return true;
    }
    auto* pWorld = data.WorldSpace ? Cast<TESWorldSpace>(TESForm::GetById(mods.GetGameId(data.WorldSpace))) : nullptr;
    if (!IsWorkshopObject(pBase) || !pWorkshop || (data.WorldSpace && !pWorld))
        return false;
    NiPoint3 position;
    position.x = data.Position.x;
    position.y = data.Position.y;
    position.z = data.Position.z;
    NiPoint3 rotation;
    rotation.x = data.Rotation.x;
    rotation.y = data.Rotation.y;
    rotation.z = data.Rotation.z;
    if (!pRef && !data.Reference)
    {
        pRef = TESObjectREFR::GetByHandle(ModManager::Get()->Spawn(position, rotation, pCell, pWorld, pBase));
        if (!pRef)
            return false;
        aRecord.Spawned = !m_isLeader;
        if (!m_isLeader)
            pRef->SetSkipSaveFlag(true);
    }
    if (!pRef || pRef->baseForm != pBase)
        return false;
    aRecord.LocalId = pRef->formID;
    using MoveSpace = void(TESObjectREFR*, TESObjectCELL*, TESWorldSpace*);
    using SetPosition = void(TESObjectREFR*, const NiPoint3&);
    using SetScale = void(TESObjectREFR*, float);
    using SetWorkshop = void(TESObjectREFR*, bool, TESObjectREFR*);
    static VersionDbPtr<MoveSpace> moveSpace(2201149);
    static VersionDbPtr<SetPosition> setPosition(2201138);
    static VersionDbPtr<SetScale> setScale(2200893);
    static VersionDbPtr<SetWorkshop> setWorkshop(2194972);
    setPosition(pRef, position);
    if (pRef->GetParentCellEx() != pCell)
        moveSpace(pRef, pWorld ? nullptr : pCell, pWorld);
    pRef->SetRotation(rotation.x, rotation.y, rotation.z);
    setScale(pRef, data.Scale / 100.f);
    pRef->Update3DPosition(true);
    setWorkshop(pRef, true, pWorkshop);
    pRef->Enable();
    if (m_isLeader)
    {
        pRef->MarkChanged(TESObjectREFR::CHANGE_REFR_MOVE | TESObjectREFR::CHANGE_REFR_SCALE | TESObjectREFR::CHANGE_REFR_CELL_CHANGED);
        if (!data.Reference && !data.LeaderReference)
        {
            auto& pending = m_pending[pRef->formID];
            pending.Data = data;
            pending.Data.LeaderReference = pRef->formID;
        }
    }
    spdlog::info("Settlement applied {} revision {} local {:X}", data.Id, data.Revision, aRecord.LocalId);
    return true;
}
#endif
