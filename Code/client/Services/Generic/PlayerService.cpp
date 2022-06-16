#include <Services/PlayerService.h>

#include <World.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/GridCellChangeEvent.h>
#include <Events/CellChangeEvent.h>
#include <Events/PlayerDialogueEvent.h>
#include <Events/PlayerMapMarkerUpdateEvent.h>
#include <Events/PlayerLevelEvent.h>
#include <Events/PlayerSetWaypointEvent.h>
#include <Events/PlayerMapMarkerUpdateEvent.h>
#include <Messages/PlayerRespawnRequest.h>
#include <Messages/NotifyPlayerRespawn.h>
#include <Messages/ShiftGridCellRequest.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/RequestSetWaypoint.h>
#include <Messages/NotifyPlayerLeft.h>
#include <Messages/NotifyPlayerJoined.h>
#include <Messages/PlayerDialogueRequest.h>
#include <Messages/PlayerLevelRequest.h>
#include <Messages/NotifyPlayerPosition.h>
#include <Messages/NotifyPlayerCellChanged.h>
#include <Messages/NotifySetWaypoint.h>

#include <Structs/ServerSettings.h>

#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Games/Overrides.h>
#include <Games/References.h>
#include <AI/AIProcess.h>
#include <Forms/TESWorldSpace.h>
#include <ExtraData/ExtraMapMarker.h>

PlayerService::PlayerService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept 
    : m_world(aWorld), m_dispatcher(aDispatcher), m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&PlayerService::OnUpdate>(this);
    m_connectedConnection = m_dispatcher.sink<ConnectedEvent>().connect<&PlayerService::OnConnected>(this);
    m_disconnectedConnection = m_dispatcher.sink<DisconnectedEvent>().connect<&PlayerService::OnDisconnected>(this);
    m_settingsConnection = m_dispatcher.sink<ServerSettings>().connect<&PlayerService::OnServerSettingsReceived>(this);
    m_playerJoinedConnection = m_dispatcher.sink<NotifyPlayerJoined>().connect<&PlayerService::OnPlayerJoined>(this);
    m_playerLeftConnection = m_dispatcher.sink<NotifyPlayerLeft>().connect<&PlayerService::OnPlayerLeft>(this);
    m_playerNotifySetWaypointConnection = m_dispatcher.sink<NotifySetWaypoint>().connect<&PlayerService::OnNotifyPlayerSetWaypoint>(this);
    m_notifyRespawnConnection = m_dispatcher.sink<NotifyPlayerRespawn>().connect<&PlayerService::OnNotifyPlayerRespawn>(this);
    m_gridCellChangeConnection = m_dispatcher.sink<GridCellChangeEvent>().connect<&PlayerService::OnGridCellChangeEvent>(this);
    m_cellChangeConnection = m_dispatcher.sink<CellChangeEvent>().connect<&PlayerService::OnCellChangeEvent>(this);
    m_playerDialogueConnection = m_dispatcher.sink<PlayerDialogueEvent>().connect<&PlayerService::OnPlayerDialogueEvent>(this);
    m_playerMapMarkerConnection = m_dispatcher.sink<PlayerMapMarkerUpdateEvent>().connect<&PlayerService::OnPlayerMapMarkerUpdateEvent>(this);
    m_playerLevelConnection = m_dispatcher.sink<PlayerLevelEvent>().connect<&PlayerService::OnPlayerLevelEvent>(this);
    m_playerPositionConnection = m_dispatcher.sink<NotifyPlayerPosition>().connect<&PlayerService::OnNotifyPlayerPosition>(this);
    m_playerCellChangeConnection = m_dispatcher.sink<NotifyPlayerCellChanged>().connect<&PlayerService::OnNotifyPlayerCellChanged>(this);
    m_playerSetWaypointConnection = m_dispatcher.sink<PlayerSetWaypointEvent>().connect<&PlayerService::OnPlayerSetWaypoint>(this);
}


// TODO: this whole thing should probably be a util function by now
TESObjectCELL* PlayerService::GetCell(const GameId& acCellId, const GameId& acWorldSpaceId, const GridCellCoords& acCenterCoords) const noexcept
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

bool DeleteMarkerDummy(const uint32_t acHandle) noexcept
{
    auto* pDummyPlayer = TESObjectREFR::GetByHandle(acHandle);
    if (!pDummyPlayer)
        return false;

    pDummyPlayer->Delete();

    PlayerCharacter::Get()->RemoveMapmarkerRef(acHandle);

    return true;
}

static bool knockdownStart = false;
static double knockdownTimer = 0.0;

static bool godmodeStart = false;
static double godmodeTimer = 0.0;

void PlayerService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    RunRespawnUpdates(acEvent.Delta);
    RunPostDeathUpdates(acEvent.Delta);
    RunDifficultyUpdates();
    RunLevelUpdates();
}

void PlayerService::OnConnected(const ConnectedEvent& acEvent) noexcept
{
    m_waypoint = TESObjectREFR::New();
    //Create Global Waypoint
    m_waypoint->SetBaseForm(TESForm::GetById(0x10));
    m_waypoint->SetSkipSaveFlag(true);

    m_waypointData = MapMarkerData::New();

    m_waypointData->name.value.Set("Custom Location");
    m_waypointData->cOriginalFlags = m_waypointData->cFlags = MapMarkerData::Flag::NONE;
    m_waypointData->sType = MapMarkerData::Type::kCustomMarker; // "custom destination" marker either 66 or 0
    m_waypoint->extraData.SetMarkerData(m_waypointData);

    uint32_t handle;
    m_waypoint->GetHandle(handle);
    PlayerCharacter::Get()->AddMapmarkerRef(handle);
}

void PlayerService::OnDisconnected(const DisconnectedEvent& acEvent) noexcept
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
        DeleteMarkerDummy(handle);
    }

    for (uint32_t playerId : toRemove)
        m_mapHandles.erase(playerId);
}

void PlayerService::OnServerSettingsReceived(const ServerSettings& acSettings) noexcept
{
    m_previousDifficulty = PlayerCharacter::Get()->difficulty;
    PlayerCharacter::Get()->SetDifficulty(acSettings.Difficulty);
    m_serverDifficulty = acSettings.Difficulty;

    if (!acSettings.GreetingsEnabled)
    {
        float* greetDistance = Settings::GetGreetDistance();
        *greetDistance = 0.f;
    }
}

void PlayerService::OnPlayerJoined(const NotifyPlayerJoined& acMessage) noexcept
{
    TESObjectREFR* pNewPlayer = TESObjectREFR::New();
    pNewPlayer->SetBaseForm(TESForm::GetById(0x10));
    pNewPlayer->SetSkipSaveFlag(true);

    TESObjectCELL* pCell = GetCell(acMessage.CellId, acMessage.WorldSpaceId, acMessage.CenterCoords);

    TP_ASSERT(pCell, "Cell not found for joined player");

    if (pCell)
        pNewPlayer->SetParentCell(pCell);

    // TODO: might have to be respawned when traveling between worldspaces?
    // doesn't always work when going from solstheim to skyrim
    MapMarkerData* pMarkerData = MapMarkerData::New();
    pMarkerData->name.value.Set(acMessage.Username.data());
    pMarkerData->cOriginalFlags = pMarkerData->cFlags = MapMarkerData::Flag::NONE;
    pMarkerData->sType = MapMarkerData::Type::kMultipleQuest; // "custom destination" marker either 66 or 0
    pNewPlayer->extraData.SetMarkerData(pMarkerData);

    uint32_t handle;
    pNewPlayer->GetHandle(handle);
    PlayerCharacter::Get()->AddMapmarkerRef(handle);

    m_mapHandles[acMessage.PlayerId] = handle;
}

void PlayerService::OnPlayerLeft(const NotifyPlayerLeft& acMessage) noexcept
{
    auto it = m_mapHandles.find(acMessage.PlayerId);
    if (it == m_mapHandles.end())
    {
        spdlog::error(__FUNCTION__ ": could not find player id {:X}", acMessage.PlayerId);
        return;
    }

    DeleteMarkerDummy(it->second);

    m_mapHandles.erase(it);
}

void PlayerService::OnNotifyPlayerRespawn(const NotifyPlayerRespawn& acMessage) const noexcept
{
    PlayerCharacter::Get()->PayGold(acMessage.GoldLost);

    std::string message = fmt::format("You died and lost {} gold.", acMessage.GoldLost);
    Utils::ShowHudMessage(String(message));
}

void PlayerService::OnGridCellChangeEvent(const GridCellChangeEvent& acEvent) const noexcept
{
    uint32_t baseId = 0;
    uint32_t modId = 0;

    if (m_world.GetModSystem().GetServerModId(acEvent.WorldSpaceId, modId, baseId))
    {
        ShiftGridCellRequest request;
        request.WorldSpaceId = GameId(modId, baseId);
        request.PlayerCell = acEvent.PlayerCell;
        request.CenterCoords = acEvent.CenterCoords;
        request.Cells = acEvent.Cells;

        m_transport.Send(request);
    }
}

void PlayerService::OnCellChangeEvent(const CellChangeEvent& acEvent) const noexcept
{
    if (acEvent.WorldSpaceId)
    {
        EnterExteriorCellRequest message;
        message.CellId = acEvent.CellId;
        message.WorldSpaceId = acEvent.WorldSpaceId;
        message.CurrentCoords = acEvent.CurrentCoords;

        m_transport.Send(message);
    }
    else
    {
        EnterInteriorCellRequest message;
        message.CellId = acEvent.CellId;

        m_transport.Send(message);
    }
}

void PlayerService::OnPlayerDialogueEvent(const PlayerDialogueEvent& acEvent) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    const auto& partyService = m_world.GetPartyService();
    if (!partyService.IsInParty() || !partyService.IsLeader())
        return;

    PlayerDialogueRequest request{};
    request.Text = acEvent.Text;

    m_transport.Send(request);
}

void PlayerService::OnNotifyPlayerPosition(const NotifyPlayerPosition& acMessage) const noexcept
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

    if (pDummyPlayer->IsInInteriorCell() || 
        (pPlayerWorldSpace && pDummyWorldSpace != pPlayerWorldSpace) ||
        (pDummyWorldSpace && pDummyWorldSpace != pPlayerWorldSpace))
    {
        pMarkerData->cOriginalFlags = pMarkerData->cFlags = MapMarkerData::Flag::NONE;
        return;
    }

    pMarkerData->cOriginalFlags = pMarkerData->cFlags = MapMarkerData::Flag::VISIBLE | MapMarkerData::Flag::CAN_TRAVEL_TO;

    pDummyPlayer->position = acMessage.Position;
}

void PlayerService::OnNotifyPlayerCellChanged(const NotifyPlayerCellChanged& acMessage) const noexcept
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

// on join/leave, add to our array...
void PlayerService::OnPlayerMapMarkerUpdateEvent(const PlayerMapMarkerUpdateEvent& acEvent) const noexcept
{
}

void PlayerService::OnPlayerLevelEvent(const PlayerLevelEvent& acEvent) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    PlayerLevelRequest request{};
    request.NewLevel = PlayerCharacter::Get()->GetLevel();

    m_transport.Send(request);
}

void PlayerService::OnPlayerSetWaypoint(const PlayerSetWaypointEvent& acMessage) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    RequestSetWaypoint request = {};
    request.Position = acMessage.Position;
    m_transport.Send(request);
}

void PlayerService::OnNotifyPlayerSetWaypoint(const NotifySetWaypoint& acMessage) const noexcept
{
    m_waypointData->cOriginalFlags = m_waypointData->cFlags = MapMarkerData::Flag::VISIBLE;
    m_waypoint->position.x = acMessage.Position.x;
    m_waypoint->position.y = acMessage.Position.y;
}

void PlayerService::RunRespawnUpdates(const double acDeltaTime) noexcept
{
    static bool s_startTimer = false;

    PlayerCharacter* pPlayer = PlayerCharacter::Get();
    if (!pPlayer->actorState.IsBleedingOut())
    {
        s_startTimer = false;
        return;
    }

    if (!s_startTimer)
    {
        s_startTimer = true;
        m_respawnTimer = 5.0;
        FadeOutGame(true, true, 3.0f, true, 2.0f);

        // If a player dies not by its health reaching 0, getting it up from its bleedout state isn't possible
        // just by setting its health back to max. Therefore, put it to 0.
        if (pPlayer->GetActorValue(ActorValueInfo::kHealth) > 0.f)
            pPlayer->ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, ActorValueInfo::kHealth, 0);

        pPlayer->PayCrimeGoldToAllFactions();
    }

    m_respawnTimer -= acDeltaTime;

    if (m_respawnTimer <= 0.0)
    {
        pPlayer->RespawnPlayer();

        knockdownTimer = 1.5;
        knockdownStart = true;

        m_transport.Send(PlayerRespawnRequest());

        s_startTimer = false;
    }
}

void PlayerService::RunPostDeathUpdates(const double acDeltaTime) noexcept
{
    // If a player dies in ragdoll, it gets stuck.
    // This code ragdolls the player again upon respawning.
    // It also makes the player invincible for 5 seconds.
    if (knockdownStart)
    {
        knockdownTimer -= acDeltaTime;
        if (knockdownTimer <= 0.0)
        {
            PlayerCharacter::SetGodMode(true);
            godmodeStart = true;
            godmodeTimer = 10.0;

            PlayerCharacter* pPlayer = PlayerCharacter::Get();
            pPlayer->currentProcess->KnockExplosion(pPlayer, &pPlayer->position, 0.f);

            FadeOutGame(false, true, 0.5f, true, 2.f);

            knockdownStart = false;
        }
    }

    if (godmodeStart)
    {
        godmodeTimer -= acDeltaTime;
        if (godmodeTimer <= 0.0)
        {
            PlayerCharacter::SetGodMode(false);

            godmodeStart = false;
        }
    }
}

void PlayerService::RunDifficultyUpdates() const noexcept
{
    if (!m_transport.IsConnected())
        return;

    PlayerCharacter::Get()->SetDifficulty(m_serverDifficulty);
}

void PlayerService::RunLevelUpdates() const noexcept
{
    // The LevelUp hook is kinda weird, so ehh, just check periodically, doesn't really cost anything.

    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenUpdates = 1000ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    static uint16_t oldLevel = PlayerCharacter::Get()->GetLevel();

    uint16_t newLevel = PlayerCharacter::Get()->GetLevel();
    if (newLevel != oldLevel)
    {
        PlayerLevelRequest request{};
        request.NewLevel = newLevel;

        m_transport.Send(request);

        oldLevel = newLevel;
    }
}
