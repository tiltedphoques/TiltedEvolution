#include <TiltedOnlinePCH.h>

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>

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
    m_leftConnection = aDispatcher.sink<DisconnectedEvent>().connect<&QuestService::OnDisconnected>(this);
    m_questUpdateConnection = aDispatcher.sink<NotifyQuestUpdate>().connect<&QuestService::OnQuestUpdate>(this);

    // bind game event listeners
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStartStopEvent.RegisterSink(this);
    pEventList->questStageEvent.RegisterSink(this);
}

//Idea: ToggleQuestActiveStatus(__int64 a1)

void QuestService::OnConnected(const ConnectedEvent&) noexcept
{
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

    // deselect any active quests
    auto* pPlayer = PlayerCharacter::Get();
    for (auto& objective : pPlayer->objectives)
    {
        if (auto* pQuest = objective.instance->quest)
            pQuest->SetActive(false);
    }
}

void QuestService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // remove quest listener
#if TP_FALLOUT
    GetEventDispatcher_TESQuestStartStopEvent()->UnRegisterSink(this);
    GetEventDispatcher_TESQuestStageEvent()->UnRegisterSink(this);
#else
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->questStageEvent.UnRegisterSink(this);
    pEventList->questStartStopEvent.UnRegisterSink(this);
#endif
}

BSTEventResult QuestService::OnEvent(const TESQuestStartStopEvent* apEvent, const EventDispatcher<TESQuestStartStopEvent>*)
{
    if (ScopedQuestOverride::IsOverriden())
        return BSTEventResult::kOk;

    if (auto* pQuest = Cast<TESQuest>(TESForm::GetById(apEvent->formId)))
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
    if (ScopedQuestOverride::IsOverriden())
        return BSTEventResult::kOk;

    spdlog::info("Quest: {:X}, stage: {}", apEvent->formId, apEvent->stageId);

    // there is no reason to even fetch the quest object, since the event provides everything already....
    if (auto* pQuest = Cast<TESQuest>(TESForm::GetById(apEvent->formId)))
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
    bool bResult = false;
    switch (aUpdate.Status)
    {
    case NotifyQuestUpdate::Started: 
    {
        if (auto* pQuest = SetQuestStage(aUpdate.Id.BaseId, 0))
        {
            pQuest->SetActive(true);
            bResult = true;
        }
        break;
    }
    case NotifyQuestUpdate::StageUpdate:
        bResult = SetQuestStage(aUpdate.Id.BaseId, aUpdate.Stage);
        break;
    case NotifyQuestUpdate::Stopped:
        bResult = StopQuest(aUpdate.Id.BaseId);
        break;
    default:
        break;
    }

    if (!bResult)
        spdlog::error("Failed to update the client quest state");
}

TESQuest* QuestService::SetQuestStage(uint32_t aFormId, uint16_t aStage)
{
    auto* pQuest = Cast<TESQuest>(TESForm::GetById(aFormId));
    if (pQuest)
    {
        // force quest update
        pQuest->flags |= TESQuest::Enabled | TESQuest::Started;
        pQuest->scopedStatus = -1;

        auto bNeedsRegistration = false;
        if (pQuest->UnkSetRunning(bNeedsRegistration, false))
        {
            auto* pCallbackMgr = QuestCallbackManager::Get();

            if (bNeedsRegistration)
                pCallbackMgr->RegisterQuest(aFormId);
            else
            {
                pQuest->SetStage(aStage);
                return pQuest;
            }
        }
    }

    return nullptr;
}

bool QuestService::StopQuest(uint32_t aformId)
{
    auto* pQuest = Cast<TESQuest>(TESForm::GetById(aformId));
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
    return apQuest->type == TESQuest::None // internal event
           || apQuest->type == TESQuest::Miscellaneous 
           || stages.Empty();
}

void QuestService::DebugDumpQuests()
{
    auto& quests = ModManager::Get()->quests;
    for (auto* it : quests)
        spdlog::info("{:X}|{}|{}|{}", it->formID, it->type, it->priority, it->idName.AsAscii());
}
