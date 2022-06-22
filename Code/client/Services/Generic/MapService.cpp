#include <Services/MapService.h>

#include <World.h>

#include <Events/CellChangeEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/MenuCloseEvent.h>
#include <Events/MenuOpenEvent.h>
#include <Events/DeleteWaypointEvent.h>
#include <Events/PlayerDialogueEvent.h>
#include <Events/PlayerMapMarkerUpdateEvent.h>
#include <Events/SetWaypointEvent.h>
#include <Events/UpdateEvent.h>
#include <Messages/NotifyDeleteWaypoint.h>
#include <Messages/NotifyPlayerCellChanged.h>
#include <Messages/NotifyPlayerJoined.h>
#include <Messages/NotifyPlayerLeft.h>
#include <Messages/NotifyPlayerPosition.h>
#include <Messages/NotifyPlayerRespawn.h>
#include <Messages/NotifySetWaypoint.h>
#include <Messages/RequestDeleteWaypoint.h>
#include <Messages/RequestSetWaypoint.h>

#include <AI/AIProcess.h>
#include <ExtraData/ExtraMapMarker.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>
#include <Games/Overrides.h>
#include <Games/References.h>
#include <Interface/Menus/MapMenu.h>
#include <Interface/UI.h>
#include <PlayerCharacter.h>

MapService::MapService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld), m_dispatcher(aDispatcher), m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&MapService::OnUpdate>(this);
    m_connectedConnection = m_dispatcher.sink<ConnectedEvent>().connect<&MapService::OnConnected>(this);
    m_disconnectedConnection = m_dispatcher.sink<DisconnectedEvent>().connect<&MapService::OnDisconnected>(this);
    m_playerJoinedConnection = m_dispatcher.sink<NotifyPlayerJoined>().connect<&MapService::OnPlayerJoined>(this);
    m_playerLeftConnection = m_dispatcher.sink<NotifyPlayerLeft>().connect<&MapService::OnPlayerLeft>(this);
    m_playerNotifySetWaypointConnection = m_dispatcher.sink<NotifySetWaypoint>().connect<&MapService::OnNotifyPlayerSetWaypoint>(this);
    m_playerNotifyDeleteWaypointConnection = m_dispatcher.sink<NotifyDeleteWaypoint>().connect<&MapService::OnNotifyPlayerDelWaypoint>(this);
    m_playerPositionConnection = m_dispatcher.sink<NotifyPlayerPosition>().connect<&MapService::OnNotifyPlayerPosition>(this);
    m_playerCellChangeConnection = m_dispatcher.sink<NotifyPlayerCellChanged>().connect<&MapService::OnNotifyPlayerCellChanged>(this);
    m_playerSetWaypointConnection = m_dispatcher.sink<SetWaypointEvent>().connect<&MapService::OnPlayerSetWaypoint>(this);
    m_playerDelWaypointConnection = m_dispatcher.sink<DeleteWaypointEvent>().connect<&MapService::OnPlayerDelWaypoint>(this);
    m_mapCloseConnection = m_dispatcher.sink<MenuCloseEvent>().connect<&MapService::OnMenuClose>(this);
}

// TODO: this whole thing should probably be a util function by now
TESObjectCELL* MapService::GetCell(const GameId& acCellId, const GameId& acWorldSpaceId,
                                      const GridCellCoords& acCenterCoords) const noexcept
{
    auto& modSystem = m_world.GetModSystem();
    uint32_t cellId = modSystem.GetGameId(acCellId);
    TESObjectCELL* pCell = Cast<TESObjectCELL>(TESForm::GetById(cellId));

    if (!pCell)
    {
        const uint32_t cWorldSpaceId = m_world.GetModSystem().GetGameId(acWorldSpaceId);
        TESWorldSpace* const pWorldSpace = Cast<TESWorldSpace>(TESForm::GetById(cWorldSpaceId));
        if (pWorldSpace)
            pCell = pWorldSpace->LoadCell(acCenterCoords.X, acCenterCoords.Y);
    }

    return pCell;
}

void MapService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    RunMapUpdates();
}

void MapService::OnConnected(const ConnectedEvent& acEvent) noexcept
{
    // Create Dummy Map Markers
    for (int i = 0; i < m_initDummyMarkers; i++)
    {
        CreateDummyMarker();
    }

    m_waypoint = TESObjectREFR::New();
    m_waypoint->SetBaseForm(TESForm::GetById(0x10));
    m_waypoint->SetSkipSaveFlag(true);
    m_waypoint->position.x = -INTMAX_MAX;
    m_waypoint->position.y = -INTMAX_MAX;

    m_waypointData = MapMarkerData::New();
    m_waypointData->name.value.Set("Custom Destination");
    m_waypointData->cOriginalFlags = m_waypointData->cFlags = MapMarkerData::Flag::VISIBLE;
    m_waypointData->sType = MapMarkerData::Type::kCustomMarker; // "custom destination" marker either 66 or 0
    m_waypoint->extraData.SetMarkerData(m_waypointData);

    uint32_t handle{};
    m_waypoint->GetHandle(handle);
    PlayerCharacter::Get()->AddMapmarkerRef(handle);
}

void MapService::OnDisconnected(const DisconnectedEvent& acEvent) noexcept
{
    auto* pPlayer = PlayerCharacter::Get();

    pPlayer->SetDifficulty(m_previousDifficulty);
    m_serverDifficulty = m_previousDifficulty = 6;

    // Restore to the default value (150)
    float* greetDistance = Settings::GetGreetDistance();
    *greetDistance = 150.f;

    TiltedPhoques::Vector<uint32_t> toRemove{};
    for (auto& [playerId, handle] : m_mapHandles)
    {
        toRemove.push_back(playerId);
        DeleteDummyMarker(handle);
    }

    for (uint32_t playerId : toRemove)
        m_mapHandles.erase(playerId);

    for (uint32_t dummyHandle : m_dummyHandles)
        DeleteDummyMarker(dummyHandle);
    m_dummyHandles.clear();

    if (m_waypoint)
        m_waypoint->Delete();
    m_waypoint = nullptr;
}


void MapService::OnPlayerJoined(const NotifyPlayerJoined& acMessage) noexcept
{
    // Associate a dummy marker with this player id
    uint32_t handle = GetDummyMarker();

    TESObjectREFR* pNewPlayer = TESObjectREFR::GetByHandle(handle);

    TESObjectCELL* pCell = GetCell(acMessage.CellId, acMessage.WorldSpaceId, acMessage.CenterCoords);

    TP_ASSERT(pCell, "Cell not found for joined player");

    if (pCell)
        pNewPlayer->SetParentCell(pCell);

    // TODO: might have to be respawned when traveling between worldspaces?
    // doesn't always work when going from solstheim to skyrim
    ExtraMapMarker* pMapMarker = Cast<ExtraMapMarker>(pNewPlayer->extraData.GetByType(ExtraData::MapMarker));
    MapMarkerData* pMarkerData = pMapMarker->pMarkerData;
    pMarkerData->name.value.Set(acMessage.Username.data());
    m_mapHandles[acMessage.PlayerId] = handle;

    // Each time a player joins create a spare dummy marker on retainer
    CreateDummyMarker();
}

void MapService::OnPlayerLeft(const NotifyPlayerLeft& acMessage) noexcept
{
    auto it = m_mapHandles.find(acMessage.PlayerId);

    // Move marker off map
    TESObjectREFR* pLeavePlayer = TESObjectREFR::GetByHandle(it->second);
    pLeavePlayer->position.x = -INTMAX_MAX;
    pLeavePlayer->position.y = -INTMAX_MAX;

    if (it == m_mapHandles.end())
    {
        spdlog::error(__FUNCTION__ ": could not find player id {:X}", acMessage.PlayerId);
        return;
    }

    DeleteDummyMarker(it->second);

    m_mapHandles.erase(it);
}

void MapService::OnMenuClose(const MenuCloseEvent& acMessage) noexcept
{
    if (!m_transport.IsConnected() || m_waypointActive || m_waypoint->position.x == -INTMAX_MAX)
        return;

    SetWaypoint(PlayerCharacter::Get(), &m_waypoint->position, PlayerCharacter::Get()->GetWorldSpace());
}

void MapService::OnNotifyPlayerPosition(const NotifyPlayerPosition& acMessage) const noexcept
{
    auto it = m_mapHandles.find(acMessage.PlayerId);
    if (it == m_mapHandles.end())
    {
        spdlog::error(__FUNCTION__ ": could not find player id {:X}", acMessage.PlayerId);
        return;
    }

    auto* pDummyPlayer = TESObjectREFR::GetByHandle(it->second);
    if (!pDummyPlayer)
    {
        spdlog::error(__FUNCTION__ ": could not find dummy player, handle: {:X}", it->second);
        return;
    }

    ExtraMapMarker* pMapMarker = Cast<ExtraMapMarker>(pDummyPlayer->extraData.GetByType(ExtraData::MapMarker));
    if (!pMapMarker || !pMapMarker->pMarkerData)
    {
        spdlog::error(__FUNCTION__ ": could not find map marker data, player id: {:X}", acMessage.PlayerId);
        return;
    }

    MapMarkerData* pMarkerData = pMapMarker->pMarkerData;

    // TODO: this is flawed due to cities being worldspaces on the same map
    auto* pDummyWorldSpace = pDummyPlayer->GetWorldSpace();
    auto* pPlayerWorldSpace = PlayerCharacter::Get()->GetWorldSpace();

    if (pDummyPlayer->IsInInteriorCell() || (pPlayerWorldSpace && pDummyWorldSpace != pPlayerWorldSpace) ||
        (pDummyWorldSpace && pDummyWorldSpace != pPlayerWorldSpace))
    {
        pMarkerData->cOriginalFlags = pMarkerData->cFlags = MapMarkerData::Flag::NONE;
        return;
    }

    pMarkerData->cOriginalFlags = pMarkerData->cFlags =
        MapMarkerData::Flag::VISIBLE | MapMarkerData::Flag::CAN_TRAVEL_TO;

    pDummyPlayer->position = acMessage.Position;
}

void MapService::OnNotifyPlayerCellChanged(const NotifyPlayerCellChanged& acMessage) const noexcept
{
    auto it = m_mapHandles.find(acMessage.PlayerId);
    if (it == m_mapHandles.end())
    {
        spdlog::error(__FUNCTION__ ": could not find player id {:X}", acMessage.PlayerId);
        return;
    }

    TESObjectCELL* pCell = GetCell(acMessage.CellId, acMessage.WorldSpaceId, acMessage.CenterCoords);
    if (!pCell)
    {
        spdlog::error(__FUNCTION__ ": could not find cell {:X}", acMessage.CellId.BaseId);
        return;
    }

    auto* pDummyPlayer = TESObjectREFR::GetByHandle(it->second);
    if (!pDummyPlayer)
    {
        spdlog::error(__FUNCTION__ ": could not find dummy player, handle: {:X}", it->second);
        return;
    }

    pDummyPlayer->SetParentCell(pCell);
}

void MapService::OnPlayerSetWaypoint(const SetWaypointEvent& acMessage) noexcept
{
    if (!m_transport.IsConnected())
        return;
    
    if (m_waypoint)
    {
        m_waypointActive = true;
        m_waypoint->position.x = -INTMAX_MAX;
        m_waypoint->position.y = -INTMAX_MAX;
    }

    RequestSetWaypoint request{};
    request.Position = acMessage.Position;
    m_transport.Send(request);
}

void MapService::OnPlayerDelWaypoint(const DeleteWaypointEvent& acMessage) noexcept
{
    if (!m_transport.IsConnected())
        return;

    m_waypointActive = false;

    RequestDeleteWaypoint request{};
    m_transport.Send(request);
}

void MapService::OnNotifyPlayerDelWaypoint(const NotifyDeleteWaypoint& acMessage) noexcept
{
    if (!m_inMap)
    {
        RemoveWaypoint(PlayerCharacter::Get());
        return;
    }

    if (m_waypoint)
    {
        m_waypoint->position.x = -INTMAX_MAX;
        m_waypoint->position.y = -INTMAX_MAX;
    }
}

void MapService::OnNotifyPlayerSetWaypoint(const NotifySetWaypoint& acMessage) noexcept
{
    if (!m_inMap)
    {
        NiPoint3 pos{};
        pos.x = acMessage.Position.x;
        pos.y = acMessage.Position.y;
        pos.z = acMessage.Position.z;

        SetWaypoint(PlayerCharacter::Get(), &pos, PlayerCharacter::Get()->GetWorldSpace());

        return;
    }

    m_waypointActive = false;
    RemoveWaypoint(PlayerCharacter::Get());

    if (m_waypoint)
        m_waypoint->position = acMessage.Position;
}


void MapService::RunMapUpdates() noexcept
{
    // Update map open status
    const VersionDbPtr<int> inMapAddr(403437);
    bool* inMap = reinterpret_cast<decltype(inMap)>(inMapAddr.Get());

    // Map Open/Close
    if (*inMap != m_inMap)
    {
        switch (*inMap)
        {

        // Map was closed
        case 0:
            World::Get().GetRunner().Trigger(MenuCloseEvent());

        // Map was opened
        case 1:
            World::Get().GetRunner().Trigger(MenuOpenEvent());
        }
    }

    m_inMap = *inMap == true;
}

bool MapService::DeleteDummyMarker(const uint32_t acHandle) noexcept
{
    auto* pDummyPlayer = TESObjectREFR::GetByHandle(acHandle);
    if (!pDummyPlayer)
        return false;

    pDummyPlayer->Delete();

    PlayerCharacter::Get()->RemoveMapmarkerRef(acHandle);

    return true;
}

void MapService::CreateDummyMarker() noexcept
{
    TESObjectREFR* dummyMark = TESObjectREFR::New();
    dummyMark->SetBaseForm(TESForm::GetById(0x10));
    dummyMark->SetSkipSaveFlag(true);
    dummyMark->position.x = -INTMAX_MAX;
    dummyMark->position.y = -INTMAX_MAX;

    MapMarkerData* dummyData = MapMarkerData::New();
    dummyData->cOriginalFlags = dummyData->cFlags = MapMarkerData::Flag::VISIBLE;
    dummyData->sType = MapMarkerData::Type::kMultipleQuest; // "custom destination" marker either 66 or 0
    dummyMark->extraData.SetMarkerData(dummyData);

    uint32_t handle{};
    dummyMark->GetHandle(handle);
    PlayerCharacter::Get()->AddMapmarkerRef(handle);

    m_dummyHandles.push_back(handle);
}

uint32_t MapService::GetDummyMarker() noexcept
{
    if (m_dummyHandles.empty())
        return m_invalidHandle;

    return m_dummyHandles.back();
}
