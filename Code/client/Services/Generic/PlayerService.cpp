#include <Services/PlayerService.h>

#include <World.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/GridCellChangeEvent.h>
#include <Events/CellChangeEvent.h>
#include <Events/PlayerDialogueEvent.h>
#include <Events/PlayerLevelEvent.h>
#include <Events/PartyJoinedEvent.h>
#include <Events/PartyLeftEvent.h>

#include <Messages/PlayerRespawnRequest.h>
#include <Messages/NotifyPlayerRespawn.h>
#include <Messages/ShiftGridCellRequest.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/PlayerDialogueRequest.h>
#include <Messages/PlayerLevelRequest.h>

#include <Structs/ServerSettings.h>

#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESGlobal.h>
#include <Games/Overrides.h>
#include <Games/References.h>
#include <AI/AIProcess.h>
#include <EquipManager.h>

PlayerService::PlayerService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept 
    : m_world(aWorld), m_dispatcher(aDispatcher), m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&PlayerService::OnUpdate>(this);
    m_connectedConnection = m_dispatcher.sink<ConnectedEvent>().connect<&PlayerService::OnConnected>(this);
    m_disconnectedConnection = m_dispatcher.sink<DisconnectedEvent>().connect<&PlayerService::OnDisconnected>(this);
    m_settingsConnection = m_dispatcher.sink<ServerSettings>().connect<&PlayerService::OnServerSettingsReceived>(this);
    m_notifyRespawnConnection = m_dispatcher.sink<NotifyPlayerRespawn>().connect<&PlayerService::OnNotifyPlayerRespawn>(this);
    m_gridCellChangeConnection = m_dispatcher.sink<GridCellChangeEvent>().connect<&PlayerService::OnGridCellChangeEvent>(this);
    m_cellChangeConnection = m_dispatcher.sink<CellChangeEvent>().connect<&PlayerService::OnCellChangeEvent>(this);
    m_playerDialogueConnection = m_dispatcher.sink<PlayerDialogueEvent>().connect<&PlayerService::OnPlayerDialogueEvent>(this);
    m_playerLevelConnection = m_dispatcher.sink<PlayerLevelEvent>().connect<&PlayerService::OnPlayerLevelEvent>(this);
    m_partyJoinedConnection = aDispatcher.sink<PartyJoinedEvent>().connect<&PlayerService::OnPartyJoinedEvent>(this);
    m_partyLeftConnection = aDispatcher.sink<PartyLeftEvent>().connect<&PlayerService::OnPartyLeftEvent>(this);
}

void PlayerService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    RunRespawnUpdates(acEvent.Delta);
    RunPostDeathUpdates(acEvent.Delta);
    RunDifficultyUpdates();
    RunLevelUpdates();
}

void PlayerService::OnConnected(const ConnectedEvent& acEvent) noexcept
{
    // TODO: SkyrimTogether.esm
    TESGlobal* pKillMove = Cast<TESGlobal>(TESForm::GetById(0x100F19));
    pKillMove->f = 0.f;

#if TP_SKYRIM64
    TESGlobal* pWorldEncountersEnabled = Cast<TESGlobal>(TESForm::GetById(0xB8EC1));
    pWorldEncountersEnabled->f = 0.f;
#elif TP_FALLOUT4
    // Makes it so that VATS doesn't slow down time
    float* vatsTargetingMult = Settings::GetVATSSelectTargetTimeMultiplier();
    *vatsTargetingMult = 0.f;

    // TODO(ft): disable world encounters
#endif
}

void PlayerService::OnDisconnected(const DisconnectedEvent& acEvent) noexcept
{
    PlayerCharacter::Get()->SetDifficulty(m_previousDifficulty);
    m_serverDifficulty = m_previousDifficulty = 6;

    ToggleDeathSystem(false);

    TESGlobal* pKillMove = Cast<TESGlobal>(TESForm::GetById(0x100F19));
    pKillMove->f = 1.f;

    // Restore to the default value (150 in skyrim, 175 in fallout 4)
    float* greetDistance = Settings::GetGreetDistance();
#if TP_SKYRIM64
    *greetDistance = 150.f;

    TESGlobal* pWorldEncountersEnabled = Cast<TESGlobal>(TESForm::GetById(0xB8EC1));
    pWorldEncountersEnabled->f = 1.f;
#elif TP_FALLOUT4
    *greetDistance = 175.f;

    // Restore VATS slow time (default is 0.04)
    float* vatsTargetingMult = Settings::GetVATSSelectTargetTimeMultiplier();
    *vatsTargetingMult = 0.04f;

    // TODO(ft): enable world encounters
#endif
}

void PlayerService::OnServerSettingsReceived(const ServerSettings& acSettings) noexcept
{
    m_previousDifficulty = *Settings::GetDifficulty();
    PlayerCharacter::Get()->SetDifficulty(acSettings.Difficulty);
    m_serverDifficulty = acSettings.Difficulty;

    if (!acSettings.GreetingsEnabled)
    {
        float* greetDistance = Settings::GetGreetDistance();
        *greetDistance = 0.f;
    }

    ToggleDeathSystem(acSettings.DeathSystemEnabled);
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

void PlayerService::OnPlayerLevelEvent(const PlayerLevelEvent& acEvent) const noexcept
{
    if (!m_transport.IsConnected())
        return;

    PlayerLevelRequest request{};
    request.NewLevel = PlayerCharacter::Get()->GetLevel();

    m_transport.Send(request);
}

void PlayerService::OnPartyJoinedEvent(const PartyJoinedEvent& acEvent) noexcept
{
    // TODO: this can be done a bit prettier
#if TP_SKYRIM64
    if (acEvent.IsLeader)
    {
        TESGlobal* pWorldEncountersEnabled = Cast<TESGlobal>(TESForm::GetById(0xB8EC1));
        pWorldEncountersEnabled->f = 1.f;
    }
#elif TP_FALLOUT4
        // TODO: ft
#endif
}

void PlayerService::OnPartyLeftEvent(const PartyLeftEvent& acEvent) noexcept
{
    // TODO: this can be done a bit prettier
#if TP_SKYRIM64
    if (World::Get().GetTransport().IsConnected())
    {
        TESGlobal* pWorldEncountersEnabled = Cast<TESGlobal>(TESForm::GetById(0xB8EC1));
        pWorldEncountersEnabled->f = 0.f;
    }
#elif TP_FALLOUT4
        // TODO: ft
#endif
}

// TODO: ft (verify)
void PlayerService::RunRespawnUpdates(const double acDeltaTime) noexcept
{
    if (!m_isDeathSystemEnabled)
        return;

    static bool s_startTimer = false;

    PlayerCharacter* pPlayer = PlayerCharacter::Get();
    if (!pPlayer->actorState.IsBleedingOut())
    {
#if TP_SKYRIM64
        m_cachedMainSpellId = pPlayer->magicItems[0] ? pPlayer->magicItems[0]->formID : 0;
        m_cachedSecondarySpellId = pPlayer->magicItems[1] ? pPlayer->magicItems[1]->formID : 0;
        m_cachedPowerId = pPlayer->equippedShout ? pPlayer->equippedShout->formID : 0;
#endif

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

        m_knockdownTimer = 1.5;
        m_knockdownStart = true;

        m_transport.Send(PlayerRespawnRequest());

        s_startTimer = false;

#if TP_SKYRIM64
        auto* pEquipManager = EquipManager::Get();
        TESForm* pSpell = TESForm::GetById(m_cachedMainSpellId);
        if (pSpell)
            pEquipManager->EquipSpell(pPlayer, pSpell, 0);
        pSpell = TESForm::GetById(m_cachedSecondarySpellId);
        if (pSpell)
            pEquipManager->EquipSpell(pPlayer, pSpell, 1);
        pSpell = TESForm::GetById(m_cachedPowerId);
        if (pSpell)
            pEquipManager->EquipShout(pPlayer, pSpell);
#endif
    }
}

// TODO: ft (verify)
// Doesn't seem to respawn quite yet
void PlayerService::RunPostDeathUpdates(const double acDeltaTime) noexcept
{
    if (!m_isDeathSystemEnabled)
        return;

    // If a player dies in ragdoll, it gets stuck.
    // This code ragdolls the player again upon respawning.
    // It also makes the player invincible for 5 seconds.
    if (m_knockdownStart)
    {
        m_knockdownTimer -= acDeltaTime;
        if (m_knockdownTimer <= 0.0)
        {
            PlayerCharacter::SetGodMode(true);
            m_godmodeStart = true;
            m_godmodeTimer = 10.0;

            PlayerCharacter* pPlayer = PlayerCharacter::Get();
            pPlayer->currentProcess->KnockExplosion(pPlayer, &pPlayer->position, 0.f);

            FadeOutGame(false, true, 0.5f, true, 2.f);

            m_knockdownStart = false;
        }
    }

    if (m_godmodeStart)
    {
        m_godmodeTimer -= acDeltaTime;
        if (m_godmodeTimer <= 0.0)
        {
            PlayerCharacter::SetGodMode(false);

            m_godmodeStart = false;
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

void PlayerService::ToggleDeathSystem(bool aSet) noexcept
{
    m_isDeathSystemEnabled = aSet;

    PlayerCharacter::Get()->SetPlayerRespawnMode(aSet);
}
