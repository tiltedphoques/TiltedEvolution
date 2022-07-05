#include <TiltedOnlinePCH.h>

#include <Events/ConnectedEvent.h>
#include <Events/TopicEvent.h>

#include <Services/QuestService.h>
#include <Services/ImguiService.h>

#include <Games/Misc/QuestCallbackManager.h>
#include <PlayerCharacter.h>
#include <Forms/TESQuest.h>
#include <Games/TES.h>
#include <Games/Overrides.h>

#include <Events/EventDispatcher.h>

#include <Messages/RequestQuestUpdate.h>
#include <Messages/NotifyQuestUpdate.h>
#include <Messages/TopicRequest.h>
#include <Messages/NotifyTopic.h>

static TESQuest* FindQuestByNameId(const String &name)
{
    auto& questRegistry = ModManager::Get()->quests;
    auto it = std::find_if(questRegistry.begin(), questRegistry.end(), [name](auto* it) 
    { 
         return std::strcmp(it->idName.AsAscii(), name.c_str()); 
    });

    return it != questRegistry.end() ? *it : nullptr;
}

QuestService::QuestService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
{
    m_joinedConnection = aDispatcher.sink<ConnectedEvent>().connect<&QuestService::OnConnected>(this);
    m_questUpdateConnection = aDispatcher.sink<NotifyQuestUpdate>().connect<&QuestService::OnQuestUpdate>(this);
    m_topicConnection = aDispatcher.sink<TopicEvent>().connect<&QuestService::OnTopicEvent>(this);
    m_topicNotifyConnection = aDispatcher.sink<NotifyTopic>().connect<&QuestService::OnNotifyTopic>(this);

    // A note about the Gameevents:
    // TESQuestStageItemDoneEvent gets fired to late, we instead use TESQuestStageEvent, because it responds immediately.
    // TESQuestInitEvent can be instead managed by start stop quest management.
#if TP_FALLOUT
    GetEventDispatcher_TESQuestStartStopEvent()->RegisterSink(this);
    GetEventDispatcher_TESQuestStageEvent()->RegisterSink(this);
#else
    // bind game event listeners
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStartStopEvent.RegisterSink(this);
    pEventList->questStageEvent.RegisterSink(this);
#endif
}

void QuestService::OnConnected(const ConnectedEvent&) noexcept
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
}

void QuestService::OnTopicEvent(const TopicEvent& acEvent) noexcept
{
    if (!m_world.GetPartyService().IsLeader())
        return;

    auto& modSystem = m_world.GetModSystem();

    auto view = m_world.view<FormIdComponent>(entt::exclude<ObjectComponent>);
    auto entityIt = std::find_if(view.begin(), view.end(), [view, formId = acEvent.SpeakerID](auto entity) {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

    if (entityIt == view.end())
        return;

    auto serverIdRes = Utils::GetServerId(*entityIt);
    if (!serverIdRes)
    {
        spdlog::error("{}: server id not found for form id {:X}", __FUNCTION__, acEvent.SpeakerID);
        return;
    }

    TopicRequest request{};
    request.SpeakerID = *serverIdRes;
    request.Type = acEvent.Type;

    modSystem.GetServerModId(acEvent.TopicID1, request.TopicID1);

    // Weird edge case where it's 0 or 0xFFFFFFFF
    if (acEvent.TopicID2 == 0 || acEvent.TopicID2 == 0xFFFFFFFF)
        request.TopicID2.BaseId = acEvent.TopicID2;
    else
        modSystem.GetServerModId(acEvent.TopicID2, request.TopicID2);

    m_world.GetTransport().Send(request);
}

void QuestService::OnNotifyTopic(const NotifyTopic& acMessage) noexcept
{
    auto remoteView = m_world.view<RemoteComponent, FormIdComponent>();
    const auto remoteIt = std::find_if(std::begin(remoteView), std::end(remoteView), [remoteView, Id = acMessage.SpeakerID](auto entity)
    {
        return remoteView.get<RemoteComponent>(entity).Id == Id;
    });

    if (remoteIt == std::end(remoteView))
    {
        spdlog::warn("Actor for topic with remote id {:X} not found.", acMessage.SpeakerID);
        return;
    }

    auto formIdComponent = remoteView.get<FormIdComponent>(*remoteIt);
    const TESForm* pForm = TESForm::GetById(formIdComponent.Id);
    TESObjectREFR* pSpeaker = Cast<TESObjectREFR>(pForm);

    if (!pSpeaker)
        return;

    TESTopicInfoEvent topicEvent{};
    topicEvent.pSpeaker = pSpeaker;
    topicEvent.eType = acMessage.Type;

    auto& modSystem = m_world.GetModSystem();

    topicEvent.topicId1 = modSystem.GetGameId(acMessage.TopicID1);

    if (acMessage.TopicID2.BaseId == 0 || acMessage.TopicID2.BaseId == 0xFFFFFFFF)
        topicEvent.topicId2 = acMessage.TopicID2.BaseId;
    else
        topicEvent.topicId2 = modSystem.GetGameId(acMessage.TopicID2);

    TESQuest::ExecuteDialogueFragment(&topicEvent);
}

BSTEventResult QuestService::OnEvent(const TESQuestStartStopEvent* apEvent, const EventDispatcher<TESQuestStartStopEvent>*)
{
    if (ScopedQuestOverride::IsOverriden()
        || !m_world.Get().GetPartyService().IsLeader())
        return BSTEventResult::kOk;

    spdlog::info("Quest start/stop event: {:X}", apEvent->formId);

    if (TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(apEvent->formId)))
    {
        if (IsNonSyncableQuest(pQuest))
            return BSTEventResult::kOk;

        m_world.GetRunner().Queue([&, 
            formId = pQuest->formID, 
            stageId = pQuest->currentStage, 
            stopped = pQuest->IsStopped()]() 
        {
            GameId Id;
            auto& modSys = m_world.GetModSystem();
            if (modSys.GetServerModId(formId, Id))
            {
                RequestQuestUpdate update;
                update.Id = Id;
                update.Stage = stageId;
                update.Status = stopped ? RequestQuestUpdate::Stopped : RequestQuestUpdate::Started;

                m_world.GetTransport().Send(update);
            }
        });
    }

    return BSTEventResult::kOk;
}

BSTEventResult QuestService::OnEvent(const TESQuestStageEvent* apEvent, const EventDispatcher<TESQuestStageEvent>*)
{
    if (ScopedQuestOverride::IsOverriden()
        || !m_world.Get().GetPartyService().IsLeader())
        return BSTEventResult::kOk;

    spdlog::info("Quest stage event: {:X}, stage: {}", apEvent->formId, apEvent->stageId);

    // there is no reason to even fetch the quest object, since the event provides everything already....
    if (TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(apEvent->formId)))
    {
        if (IsNonSyncableQuest(pQuest))
            return BSTEventResult::kOk;

        m_world.GetRunner().Queue([&, 
            formId = apEvent->formId, 
            stageId = apEvent->stageId]() 
        {
            GameId Id;
            auto& modSys = m_world.GetModSystem();
            if (modSys.GetServerModId(formId, Id))
            {
                RequestQuestUpdate update;
                update.Id = Id;
                update.Stage = stageId;
                update.Status = RequestQuestUpdate::StageUpdate;

                m_world.GetTransport().Send(update);
            }
        });
    }

    return BSTEventResult::kOk;
}

void QuestService::OnQuestUpdate(const NotifyQuestUpdate& aUpdate) noexcept
{
    ModSystem& modSystem = World::Get().GetModSystem();
    uint32_t formId = modSystem.GetGameId(aUpdate.Id);

    bool bResult = false;
    switch (aUpdate.Status)
    {
    case NotifyQuestUpdate::Started: 
    {
        if (TESQuest* pQuest = SetQuestStage(formId, aUpdate.Stage))
        {
            pQuest->SetActive(true);
            bResult = true;
            spdlog::info("Remote quest started: {:X}, stage: {}", formId, aUpdate.Stage);
        }
        break;
    }
    case NotifyQuestUpdate::StageUpdate:
        bResult = SetQuestStage(formId, aUpdate.Stage);
        spdlog::info("Remote quest updated: {:X}, stage: {}", formId, aUpdate.Stage);
        break;
    case NotifyQuestUpdate::Stopped:
        bResult = StopQuest(formId);
        spdlog::info("Remote quest stopped: {:X}, stage: {}", formId, aUpdate.Stage);
        break;
    default:
        break;
    }

    if (!bResult)
        spdlog::error("Failed to update the client quest state, quest: {:X}, stage: {}, status: {}", formId, aUpdate.Stage, aUpdate.Status);
}

TESQuest* QuestService::SetQuestStage(uint32_t aFormId, uint16_t aStage)
{
    TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(aFormId));
    if (pQuest)
    {
        // force quest update
        pQuest->flags |= TESQuest::Enabled | TESQuest::StageWait;
        pQuest->scopedStatus = -1;

        bool bNeedsRegistration = false;
        if (pQuest->EnsureQuestStarted(bNeedsRegistration, false))
        {
            auto* pCallbackMgr = QuestCallbackManager::Get();

            if (bNeedsRegistration)
                pCallbackMgr->RegisterQuest(aFormId);
            else
            {
                ScopedQuestOverride _;

                pQuest->ScriptSetStage(aStage);
                return pQuest;
            }
        }
    }

    return nullptr;
}

bool QuestService::StopQuest(uint32_t aformId)
{
    TESQuest* pQuest = Cast<TESQuest>(TESForm::GetById(aformId));
    if (pQuest)
    {
        pQuest->SetActive(false);
        pQuest->SetStopped();
        return true;
    }

    return false;
}

bool QuestService::IsNonSyncableQuest(TESQuest* apQuest)
{
    // non story quests are "blocked" and not synced
    auto& stages = apQuest->stages;
    return apQuest->type == TESQuest::Type::None // internal event
           || apQuest->type == TESQuest::Type::Miscellaneous 
           || stages.Empty();
}

void QuestService::DebugDumpQuests()
{
    auto& quests = ModManager::Get()->quests;
    for (TESQuest* pQuest : quests)
        spdlog::info("{:X}|{}|{}|{}", pQuest->formID, (uint8_t)pQuest->type, pQuest->priority, pQuest->idName.AsAscii());
}
