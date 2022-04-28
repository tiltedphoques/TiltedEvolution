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
        // Make bleedout state recoverable
        pPlayer->SetNoBleedoutRecovery(false);

        pPlayer->DispellAllSpells();

        // Reset health to max
        // TODO(cosideci): there's a cleaner way to do this
        pPlayer->ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, ActorValueInfo::kHealth, 1000000);

        TESObjectCELL* pCell = nullptr;
        NiPoint3 pos{};

        if (pPlayer->GetWorldSpace())
        {
            // TP to Whiterun temple when killed in world space
            pCell = Cast<TESObjectCELL>(TESForm::GetById(0x165aa));
            pos.x = 379.915f;
            pos.y = -381.969f;
            pos.z = -223.650f;
        }
        else
        {
            // TP to start of cell when killed in an interior
            pCell = pPlayer->GetParentCell();
            NiPoint3 rot{};
            pCell->GetCOCPlacementInfo(&pos, &rot, true);
        }

        pPlayer->MoveTo(pCell, pos);

        // Make bleedout state unrecoverable again for when the player goes down the next time
        pPlayer->SetNoBleedoutRecovery(true);

        s_startTimer = false;
    }
}
