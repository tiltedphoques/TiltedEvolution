#include <Services/PlayerService.h>

#include <World.h>

#include <Events/UpdateEvent.h>

#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>

PlayerService::PlayerService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept 
    : m_world(aWorld), m_dispatcher(aDispatcher), m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&PlayerService::OnUpdate>(this);
}

void PlayerService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    RunRespawnUpdates(acEvent.Delta);
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

        // If a player dies not by its health reaching 0, getting it up from its bleedout state isn't possible
        // just by setting its health back to max. Therefore, put it to 0.
        if (pPlayer->GetActorValue(ActorValueInfo::kHealth) > 0.f)
            pPlayer->ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, ActorValueInfo::kHealth, 0);
    }

    m_respawnTimer -= acDeltaTime;

    if (m_respawnTimer <= 0.0)
    {
        pPlayer->RespawnPlayer();
        s_startTimer = false;
    }
}
