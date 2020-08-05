
#include <GameServer.h>

#include <Services/EnvironmentService.h>
#include <Events/UpdateEvent.h>
#include <Events/CharacterSpawnedEvent.h>
#include <Messages/ServerTimeSettings.h>
#include <Components.h>

EnvironmentService::EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher) : m_World(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::OnUpdate>(this);
    m_joinConnection = aDispatcher.sink<CharacterSpawnedEvent>().connect<&EnvironmentService::OnCharSpawn>(this);
}

void EnvironmentService::OnCharSpawn(const CharacterSpawnedEvent &acEvent) noexcept
{
    auto *pServer = GameServer::Get();
    ServerTimeSettings timeMsg;
    timeMsg.ServerTick = pServer->GetTickRate();
    timeMsg.TimeScale = m_fTimeScale;
    timeMsg.Time = m_fTime;

    const auto &characterCellIdComponent = m_World.get<CellIdComponent>(acEvent.Entity);
    const auto &characterOwnerComponent = m_World.get<OwnerComponent>(acEvent.Entity);

    const auto view = m_World.view<PlayerComponent, CellIdComponent>();

    for (auto entity : view)
    {
        auto &playerComponent = view.get<PlayerComponent>(entity);
        auto &cellIdComponent = view.get<CellIdComponent>(entity);

        if (characterOwnerComponent.ConnectionId == playerComponent.ConnectionId ||
            characterCellIdComponent.CellId != cellIdComponent.CellId)
            continue;

        pServer->Send(playerComponent.ConnectionId, timeMsg);
    }
}

bool EnvironmentService::SetTime(int iHour, int iMinute, float fScale)
{
    m_fTimeScale = fScale;

    if (iHour >= 0 && iHour <= 24 && iMinute >= 0 && iMinute <= 60)
    {
        // encode time as skyrim time
        // this is technically not 100% accurate, but since
        // every client gets this applied it should be fine?
        float fMinute = static_cast<float>(iMinute) * 0.17f;
        fMinute = floor(fMinute * 100) / 1000;
        m_fTime = static_cast<float>(iHour) + fMinute;

        auto *pServer = GameServer::Get();
        ServerTimeSettings timeMsg;
        timeMsg.ServerTick = pServer->GetTickRate();
        timeMsg.TimeScale = m_fTimeScale;
        timeMsg.Time = m_fTime;
        pServer->SendToLoaded(timeMsg);
        return true;
    }

    return false;
}

std::pair<int, int> EnvironmentService::GetTime()
{
    float fHour = floor(m_fTime);
    float fMin = (m_fTime - fHour) / 17.f;

    int iHour = static_cast<int>(fHour);
    int iMinutes = static_cast<int>(ceil((fMin * 100.f) * 10.f));

    return {iHour, iMinutes};
}

static const int cDayLengthArray[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int GetNumerOfDaysByMonthIndex(int index)
{
    if (index <= 12)
    {
        return cDayLengthArray[index];
    }

    return 0;
}

void EnvironmentService::OnUpdate(const UpdateEvent &aEvent) noexcept
{
    // update server time projection
    m_fTime = (aEvent.Delta * m_fTimeScale) + m_fTime;
    if (m_fTime > 24.f)
    {
        int iDayLimit = GetNumerOfDaysByMonthIndex(m_iMonth);

        while (m_fTime > 24.f)
        {
            m_fTime = m_fTime + -24.f;
            m_iDay++;
        }

        if (m_iDay > iDayLimit)
        {
            m_iMonth++;
            m_iDay = m_iDay - iDayLimit;

            if (m_iMonth > 12)
            {
                m_iMonth = m_iMonth + -12;
                m_iYear++;
            }
        }
    }

#if 0
    int x, y;
    GetTime(x, y);
    std::printf("Current time projection %f the current time is %d:%d\n", m_fTime, x , y);
#endif
}
