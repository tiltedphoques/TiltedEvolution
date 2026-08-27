#include <TiltedOnlinePCH.h>

#include <Events/ConnectedEvent.h>

#include <Services/QuestService.h>
#include <Services/ImguiService.h>

#include <PlayerCharacter.h>
#include <Forms/TESQuest.h>
#include <Games/TES.h>
#include <Games/Overrides.h>
#include <AI/Movement/PlayerControls.h>

#include <Events/EventDispatcher.h>

#include <Messages/RequestQuestUpdate.h>
#include <Messages/RequestQuestSceneUpdate.h>
#include <Messages/NotifyQuestUpdate.h>
#include <Messages/NotifyQuestSceneUpdate.h>

namespace spdfmt = spdlog::fmt_lib;

static TESQuest* FindQuestByNameId(const String& name)
{
    auto& questRegistry = ModManager::Get()->quests;
    auto it = std::find_if(questRegistry.begin(), questRegistry.end(), [name](auto* it) { return std::strcmp(it->idName.AsAscii(), name.c_str()); });

    return it != questRegistry.end() ? *it : nullptr;
}

QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld)
{
    m_joinedConnection = aDispatcher.sink<ConnectedEvent>().connect<&QuestService::OnConnected>(this);
    m_questUpdateConnection = aDispatcher.sink<NotifyQuestUpdate>().connect<&QuestService::OnQuestUpdate>(this);
    m_questSceneUpdateConnection = aDispatcher.sink<NotifyQuestSceneUpdate>().connect<&QuestService::OnQuestSceneUpdate>(this);
    m_playerId = 0;

    // A note about the Gameevents:
    // TESQuestStageItemDoneEvent gets fired too late, we instead use TESQuestStageEvent, because it responds immediately.
    // TESQuestInitEvent can be instead managed by start stop quest management.
    // bind game event listeners
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStartStopEvent.RegisterSink(this);
    pEventList->questStageEvent.RegisterSink(this);

    pEventList->scenePhaseEvent.RegisterSink(this);
    pEventList->sceneActionEvent.RegisterSink(this);
    pEventList->sceneEvent.RegisterSink(this);
}

void QuestService::OnConnected(const ConnectedEvent& apEvent) noexcept
{
    // TODO: this should be followed with whatever the quest leader selected
    /*
    // deselect any active quests
    auto* pPlayer = PlayerCharacter::Get();
    for (auto& objective : pPlayer->objectives)
    {
        if (auto* pQuest = objective.instance->quest)
            pQuest->SetActive(false);
    }
    */
    m_playerId = apEvent.PlayerId;
}

BSTEventResult QuestService::OnEvent(const TESQuestStartStopEvent* apEvent,
                                     const EventDispatcher<TESQuestStartStopEvent>*)
{
    GameId Id;
    TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(apEvent->formId));
    if (pQuest == nullptr || IsNonSyncableQuest(pQuest) || !m_world.Get().GetPartyService().IsInParty() || !m_world.GetModSystem().GetServerModId(pQuest->formID, Id))
        return BSTEventResult::kOk; // pQuest == nullptr shouldn't happen, nor should getting GameId fail.

    const auto startStop = pQuest->IsStopped() ? "stopped" : "started";
    const bool isMiscNone = (pQuest->type == TESQuest::Type::None || pQuest->type == TESQuest::Type::Miscellaneous);
    const TiltedPhoques::String miscQuest(isMiscNone ? spdfmt::format("none/misc quest gameId {:X}", Id.LogFormat()) : "quest");
    const bool isLeader = m_world.Get().GetPartyService().IsLeader();
    const auto playerString = isLeader ? "leader" : "player";

    spdlog::info(
        __FUNCTION__ ": {} {} formId: {:X}, questStage: {}, questType: {}, flags: {:X}, {} {}, name: {}", startStop, miscQuest, apEvent->formId, pQuest->currentStage,
        static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());

    m_world.GetRunner().Queue(
        [&, formId = pQuest->formID, stageId = pQuest->currentStage, stopped = pQuest->IsStopped(), type = pQuest->type]()
        {
            GameId Id;
            auto& modSys = m_world.GetModSystem();
            if (modSys.GetServerModId(formId, Id))
            {
                RequestQuestUpdate update;
                update.Id = Id;
                update.Stage = stageId;
                update.Status = stopped ? RequestQuestUpdate::Stopped : RequestQuestUpdate::Started;
                update.ClientQuestType = static_cast<std::underlying_type_t<TESQuest::Type>>(type);
                m_world.GetTransport().Send(update);
            }
        });

    return BSTEventResult::kOk;
}

BSTEventResult QuestService::OnEvent(const TESQuestStageEvent* apEvent, const EventDispatcher<TESQuestStageEvent>*)
{
    GameId Id;
    TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(apEvent->formId));
    if (pQuest == nullptr || IsNonSyncableQuest(pQuest) || !m_world.Get().GetPartyService().IsInParty() || !m_world.GetModSystem().GetServerModId(pQuest->formID, Id))
        return BSTEventResult::kOk; // pQuest == nullptr shouldn't happen, nor should getting GameId fail.

    const auto startStop = pQuest->IsStopped() ? "stopped" : "started";
    const bool isMiscNone = (pQuest->type == TESQuest::Type::None || pQuest->type == TESQuest::Type::Miscellaneous);
    const TiltedPhoques::String miscQuest(isMiscNone ? spdfmt::format("none/misc quest gameId {:X}", Id.LogFormat()) : "quest");
    const bool isLeader = m_world.Get().GetPartyService().IsLeader();
    const auto playerString = isLeader ? "leader" : "player";

    spdlog::info(
        __FUNCTION__ ": stage update {} formId: {:X}, questStage: {}, questType: {}, isStopped: {}, flags: {:X}, {} {}, name: {}", miscQuest, apEvent->formId, apEvent->stageId,
        static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->IsStopped(), pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());

    // We should not see a stage 0 Update to a started quest, unless it started a long time ago in StageWait.
    bool isResetUpdate = apEvent->stageId == 0 &&
        (pQuest->flags & (TESQuest::Flags::Enabled | TESQuest::Flags::StageWait)) != (TESQuest::Flags::Enabled | TESQuest::Flags::StageWait);
    if (isResetUpdate)
        spdlog::critical(
            __FUNCTION__ ": suspected quest reset event NEEDS TEST, quest formId: {:X}, questStage: {}, questType: {}, isStopped: {}, flags {:X}, {} {}, name: {}",
            apEvent->formId, apEvent->stageId, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->IsStopped(), pQuest->flags, playerString, PlayerId(),
            pQuest->fullName.value.AsAscii());

    m_world.GetRunner().Queue(
        [&, formId = apEvent->formId, stageId = apEvent->stageId, isReset = isResetUpdate, type = pQuest->type]()
        {
            GameId Id;
            auto& modSys = m_world.GetModSystem();
            if (modSys.GetServerModId(formId, Id))
            {
                RequestQuestUpdate update;
                update.Id = Id;
                update.Stage = stageId;
                // FIXME EXPERIMENT. If the critical log fires, we think it is a reset. If observed in real life
                // we can debug it and see if this reset logic actually works
                update.Status = isReset ? RequestQuestUpdate::Reset : RequestQuestUpdate::StageUpdate;
                update.ClientQuestType = static_cast<std::underlying_type_t<TESQuest::Type>>(type);
                m_world.GetTransport().Send(update);
            }
        });

    return BSTEventResult::kOk;
}

BSTEventResult QuestService::OnEvent(const TESSceneEvent* apEvent, const EventDispatcher<TESSceneEvent>*)
{
    GameId Id;
    auto pScene = Cast<BGSScene>(TESForm::GetById(apEvent->sceneFormId));
    auto pQuest = pScene->owningQuest;
    if (pQuest == nullptr || QuestService::IsNonSyncableQuest(pQuest) || !m_world.Get().GetPartyService().IsInParty() || !m_world.GetModSystem().GetServerModId(pQuest->formID, Id))
        return BSTEventResult::kOk;

    const auto beginEnd = apEvent->sceneType ? "End" : "Begin";
    const bool isMiscNone = (pQuest->type == TESQuest::Type::None || pQuest->type == TESQuest::Type::Miscellaneous);
    const TiltedPhoques::String miscQuest(isMiscNone ? spdfmt::format("none/misc quest gameId {:X},", Id.LogFormat()) : "quest");
    const bool isLeader = m_world.Get().GetPartyService().IsLeader();
    const auto playerString = isLeader ? "leader" : "player";
    const auto isPlaying = pScene->isPlaying;

    spdlog::debug(
        __FUNCTION__ "::TESSceneEvent*"
                     ": sending scene {}, scene {:X}, isPlaying {}, {} formId: {:X}, questStage: {}, "
                     "questType: {}, isStopped: {}, flags: {:X}, {} {}, name: {}",
        beginEnd, apEvent->sceneFormId, isPlaying, miscQuest, pQuest->formID, pQuest->currentStage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type),
        pQuest->IsStopped(), pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());

    // FIXME EXPERIMENT, seems to work, scene begin/end sent to server so we can choose to send a quest stage poke.
    m_world.GetRunner().Queue(
        [&, sceneId = apEvent->sceneFormId, questId = pQuest->formID, stageId = pQuest->currentStage, type = pQuest->type, sceneType = apEvent->sceneType]()
        {
            GameId sceneGameId;
            GameId questGameId;
            auto& modSys = m_world.GetModSystem();
            if (modSys.GetServerModId(sceneId, sceneGameId) && modSys.GetServerModId(questId, questGameId))
            {
                RequestQuestSceneUpdate update;
                update.SceneId = sceneGameId;
                update.QuestId = questGameId;
                update.Stage = stageId;
                update.Status = stageId == RequestQuestUpdate::StageUpdateForced;
                update.ClientQuestType = static_cast<std::underlying_type_t<TESQuest::Type>>(type);
                update.SceneType = sceneType;

                m_world.GetTransport().Send(update);
            }
        });

    return BSTEventResult::kOk;
}

BSTEventResult QuestService::OnEvent(const TESSceneActionEvent* apEvent, const EventDispatcher<TESSceneActionEvent>*)
{
    const bool isLeader = m_world.GetPartyService().IsLeader();
    const auto playerType = isLeader ? "leaderId" : "playerId";

    spdlog::debug("TESSceneActionEvent: questId {:X}, scene id {:X}, action index {}, {} {}", apEvent->questFormId,
                 apEvent->sceneFormId, apEvent->actionIndex, playerType, PlayerId());
    return BSTEventResult::kOk;
}

BSTEventResult QuestService::OnEvent(const TESScenePhaseEvent* apEvent, const EventDispatcher<TESScenePhaseEvent>*)
{
    auto pScene = Cast<BGSScene>(TESForm::GetById(apEvent->sceneFormId));
    auto pQuest = pScene ? pScene->owningQuest : nullptr;
    const String sceneType = apEvent->sceneType == 0 ? "Begin" : "End";
    const bool isLeader = m_world.GetPartyService().IsLeader();
    const auto playerType = isLeader ? "leaderId" : "playerId";
    const auto isAnyScenePlaying = pQuest == nullptr ? false : pQuest->IsAnyCutscenePlaying();

    spdlog::debug(
        "TESScenePhaseEvent event: quest {:X}, stage {}, scene {:X}. phase index {}, type {}, "
        "isAnyScenePlaying {},by {} {}",
        pQuest ? pQuest->formID : 0, apEvent->questStageId, apEvent->sceneFormId, apEvent->phaseIndex, sceneType, isAnyScenePlaying, playerType, PlayerId());

    // Failsafe if we did not get a scene Begin event; happens if waiting
    // in Phase 0 (Phase 1 in CK), which means the first thing we'll get
    // is Phase 0 End. And on very long scenes, refreshes SceneMaster so it doesn't time out.
    const TESSceneEvent event = {nullptr, apEvent->sceneFormId, 0};
    if (apEvent->sceneType == 0 || apEvent->phaseIndex == 0)
        OnEvent(&event, nullptr);

    return BSTEventResult::kOk;
}

void QuestService::OnQuestUpdate(const NotifyQuestUpdate& aUpdate) noexcept
{
    auto Id = aUpdate.Id;
    auto formId = World::Get().GetModSystem().GetGameId(aUpdate.Id);
    TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(formId));
    if (pQuest == nullptr || !m_world.Get().GetPartyService().IsInParty())
        return;

    const auto startStop = pQuest->IsStopped() ? "stopped" : "started";
    const bool isMiscNone = (pQuest->type == TESQuest::Type::None || pQuest->type == TESQuest::Type::Miscellaneous); // If we can't get the GameId we can't sync anyway.
    const TiltedPhoques::String miscQuest(isMiscNone ? spdfmt::format("none/misc quest gameId {:X},", Id.LogFormat()) : "quest");
    const bool isLeader = m_world.Get().GetPartyService().IsLeader();
    const auto playerString = isLeader ? "leader" : "player";
    const auto isSceneMaster = aUpdate.SceneMaster == PlayerId();

    // Quest OnEvent()s send updates to the server where the Leader deduplicates them,
    // so most remote updates (QuestService::OnQuestUpdate) are coming from the Leader.
    //
    // But when Party Members are all playing a scene, the scene itself is advancing
    // the quest, so everyone is sending the same quest updates close enough together some
    // can sneak through the dedup code. That's solved client-side by not allowing rewind
    // when in a scene.
    //
    // The initiator of the scene is remembered as SceneMaster and sent in NotifyQuestUpdate.
    // Currently only used in experiments & logs, we may end up needing it.
    //
    // FIXME: Experiment: To unstick non-SceneMasters if they get stuck on an interaction,
    // the SceneMaster sends a forced quest stage update at the end of the scene. This may
    // be unnecessary now that we have fixed scene playing speed sync to be better.
    bool updateDisabled = pQuest->IsAnyCutscenePlaying() && (aUpdate.Status == NotifyQuestUpdate::StageUpdate && aUpdate.Stage < pQuest->currentStage);

    if (updateDisabled)
    {
        spdlog::debug(
            __FUNCTION__ ": suppressing quest stage update while playing a scene: gameId: {:X}, formId: {:X}, "
                         "questStage: {}, questStatus: {}, questType: {}, SceneMaster {}, "
                         "isAnyCutscenePlaying {}, player {}, formId: {:X}, name: {}",
            aUpdate.Id.LogFormat(), formId, aUpdate.Stage, aUpdate.Status, aUpdate.ClientQuestType, PlayerId() == aUpdate.SceneMaster, pQuest->IsAnyCutscenePlaying(), PlayerId(),
            formId, pQuest->fullName.value.AsAscii());
        return;
    }

    bool wasUpdated = false;
    const bool isRunning = pQuest->getState() == TESQuest::State::Running;
    switch (aUpdate.Status)
    {
    case NotifyQuestUpdate::Started:
        if (isRunning)
        {
            spdlog::debug(
                __FUNCTION__ ": suppressing duplicate start {} formId: {:X}, questStage: {}, "
                             "questType: {}, isStopped: {}, flags: {:X}, {} {}, name: {}",
                miscQuest, formId, aUpdate.Stage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->IsStopped(),
                pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());
        }
        else
        {
            spdlog::debug(
                __FUNCTION__ ": remotely started {} formId: {:X}, questStage: {}, "
                             "questType: {}, isStopped: {}, flags: {:X}, {} {}, name: {}",
                miscQuest, formId, aUpdate.Stage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->IsStopped(),
                pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());

            pQuest->ScriptSetStage(aUpdate.Stage);
            pQuest->SetActive(true);
        }
        wasUpdated = true;
        break;

    case NotifyQuestUpdate::Reset:
        spdlog::critical(
            __FUNCTION__ ": REPORT THIS LOG, experimental remote reset {} formId: {:X}, questStage: {}, questType: {}, "
                         "isStopped: {}, flags: {:X}, {} {}, name: {}",
            miscQuest, formId, aUpdate.Stage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->IsStopped(),
            pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());

        pQuest->ScriptResetAndUpdate();
        wasUpdated = true;
        break;

    case NotifyQuestUpdate::StageUpdate: // TODO? Insert START iff needed?
        spdlog::debug(
            __FUNCTION__ ": remotely updated {} formId: {:X}, questStage: {}, questType: {}, "
                         "isStopped: {}, flags: {:X}, {} {}, name: {}",
            miscQuest, formId, aUpdate.Stage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->IsStopped(),
            pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());

        wasUpdated = pQuest->ScriptSetStage(aUpdate.Stage);
        break;

    case NotifyQuestUpdate::Stopped:
        spdlog::debug(
            __FUNCTION__ ": remotely stopped {} formId: {:X}, questStage: {}, questType: {}, "
                         "isStopped: {}, flags: {:X}, {} {}, name: {}",
            miscQuest, formId, aUpdate.Stage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type), pQuest->IsStopped(),
            pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());
        wasUpdated = StopQuest(formId);
        break;

    default:
        break;
        spdlog::error(
            __FUNCTION__ ": unknown remote status {} {} formId: {:X}, questStage: {}, "
                         "questType: {}, isStopped: {}, flags: {:X}, {} {}, name: {}",
            aUpdate.Status, miscQuest, formId, aUpdate.Stage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type),
            pQuest->IsStopped(), pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());
    }

    if (!wasUpdated)
        spdlog::error(
            __FUNCTION__ ": failed to remotely update status {} {} formId: {:X}, questStage: "
                         "{}, questType: {}, isStopped: {}, flags: {:X}, {} {}, name: {}",
            aUpdate.Status, miscQuest, formId, aUpdate.Stage, static_cast<std::underlying_type_t<TESQuest::Type>>(pQuest->type),
            pQuest->IsStopped(), pQuest->flags, playerString, PlayerId(), pQuest->fullName.value.AsAscii());
}

void QuestService::OnQuestSceneUpdate(const NotifyQuestSceneUpdate& aUpdate) noexcept
{
    ModSystem& modSystem = World::Get().GetModSystem();
    auto pQuest = Cast<TESQuest>(TESForm::GetById(modSystem.GetGameId(aUpdate.QuestId)));
    auto pScene = Cast<BGSScene>(TESForm::GetById(modSystem.GetGameId(aUpdate.SceneId)));
    const bool bIsLeader = m_world.GetPartyService().IsLeader();
    const auto playerType = bIsLeader ? "leaderId" : "playerId";

    if (!pQuest)
    {
        spdlog::error(__FUNCTION__ ": failed to find questId: {:X}, {} {}", aUpdate.QuestId.LogFormat(), playerType, PlayerId());
        return;
    }
    if (!pScene)
    {
        spdlog::error(__FUNCTION__ ": failed to find sceneId: {:X}, {} {}", aUpdate.SceneId.LogFormat(), playerType, PlayerId());
        return;
    }

    if (aUpdate.SceneType == 0) // Scene Begin
    {
        if (pScene->isPlaying)
            spdlog::debug(
                __FUNCTION__ ": skip starting scene already playing, questId {:X}, sceneId {:X}, {} {}", aUpdate.QuestId.LogFormat(), aUpdate.SceneId.LogFormat(), playerType,
                PlayerId());
        else
        {
            spdlog::debug(
                __FUNCTION__ ": BLOCKED starting scene, causes more bugs than fixed, questId {:X}, sceneId {:X}, {} {}", aUpdate.QuestId.LogFormat(), aUpdate.SceneId.LogFormat(), playerType, PlayerId());
            // pScene->ScriptForceStart();
        }
    }

    else // Scene End
    {
        if (!pScene->isPlaying)
            spdlog::debug(
                __FUNCTION__ ": skip stopping scene not playing, questId {:X}, sceneId {:X}, {} {}", aUpdate.QuestId.LogFormat(), aUpdate.SceneId.LogFormat(), playerType,
                PlayerId());
        else
        {
            spdlog::warn(__FUNCTION__ ": FIXME/REVIEW stopping scene questId {:X}, sceneId {:X}, {} {}", aUpdate.QuestId.LogFormat(), aUpdate.SceneId.LogFormat(), playerType, PlayerId());
            pScene->ScriptStop();
        }
    }
}

bool QuestService::StopQuest(uint32_t aformId)
{
    TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(aformId));
    if (pQuest)
    {
        if (pQuest->getState() == TESQuest::State::Stopped) // Supress duplicate or loopback quest stop
        {
            spdlog::debug(
                __FUNCTION__ ": suppressing duplicate quest stop formId: {:X}, questStage: {}, questFlags: {:X}, questType: {}, formId: {:X}, name: {}", aformId,
                pQuest->currentStage, static_cast<uint8_t>(pQuest->flags), static_cast<uint16_t>(pQuest->type), aformId, pQuest->fullName.value.AsAscii());
        }
        else
        {
            pQuest->SetActive(false);
            pQuest->SetStopped();
        }

        return true;
    }

    return false;
}

static constexpr std::array kNonSyncableQuestIds = std::to_array<uint32_t>({
    0x2BA16,   // Werewolf transformation quest
    0x20071D0, // Vampire transformation quest
    0x3AC44,   // MS13BleakFallsBarrowLeverScene
    // 0xFE014801,  // Unknown dynamic ID, kept as note, maybe lookup correct ID this game?
    0xF2593 // Skill experience quest
});

bool QuestService::IsNonSyncableQuest(TESQuest* apQuest)
{
    // Quests with no quest stages are never synced (check both lists). 
    // Most TESQues::Type:: quests should be synced, including Type::None 
    // and Type::Miscellaneous, but there are a few // known exceptions to exclude.
    bool noStages = !apQuest->pExecutedStages || apQuest->pExecutedStages->Empty();
    noStages = noStages || !apQuest->pWaitingStages || apQuest->pWaitingStages->Empty();
    return noStages || std::find(kNonSyncableQuestIds.begin(), kNonSyncableQuestIds.end(), apQuest->formID) != kNonSyncableQuestIds.end();
}

void QuestService::DebugDumpQuests()
{
    auto& quests = ModManager::Get()->quests;
    for (TESQuest* pQuest : quests)
        spdlog::info("{:X}|{}|{}|{}", pQuest->formID, (uint8_t)pQuest->type, pQuest->priority, pQuest->idName.AsAscii());
}
