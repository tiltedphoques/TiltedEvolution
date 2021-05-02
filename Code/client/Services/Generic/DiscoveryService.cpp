#include <TiltedOnlinePCH.h>

#include <Services/DiscoveryService.h>
#include <Games/TES.h>

#include <Games/References.h>

#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

#include <Events/ReferenceAddedEvent.h>
#include <Events/ReferenceRemovedEvent.h>
#include <Events/PreUpdateEvent.h>
#include <Events/WorldSpaceChangeEvent.h>
#include <Events/CellChangeEvent.h>
#include <Events/LocationChangeEvent.h>
#include <Events/ConnectedEvent.h>

#include <World.h>


DiscoveryService::DiscoveryService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
{
    m_preUpdateConnection = m_dispatcher.sink<PreUpdateEvent>().connect<&DiscoveryService::OnUpdate>(this);
    m_connectedConnection = m_dispatcher.sink<ConnectedEvent>().connect<&DiscoveryService::OnConnected>(this);
}

void DiscoveryService::VisitCell(bool aForceTrigger) noexcept
{
    const auto pPlayer = PlayerCharacter::Get();
    if (!pPlayer)
        return;

    if (const auto pWorldSpace = pPlayer->GetWorldSpace())
    {
        m_interiorCellId = 0;
        const auto worldSpaceId = pWorldSpace->formID;
        if (m_worldSpaceId != worldSpaceId || aForceTrigger)
        {
            WorldSpaceChangeEvent changeEvent(worldSpaceId);

            const auto* pTES = TES::Get();
            auto* pDataHandler = DataHandler::Get();

            const auto startGridX = pTES->centerGridX - m_gridsToLoad / 2;
            const auto startGridY = pTES->centerGridY - m_gridsToLoad / 2;
            for (int32_t i = 0; i < m_gridsToLoad; ++i)
            {
                for (int32_t j = 0; j < m_gridsToLoad; ++j)
                {
                    const auto* pCell = DataHandler::GetCellFromCoordinates(pDataHandler, startGridX + i, startGridY + j, pWorldSpace, 0);

                    if (pCell)
                    {
                        uint32_t baseId = 0;
                        uint32_t modId = 0;
                        if (m_world.GetModSystem().GetServerModId(pCell->formID, modId, baseId))
                            changeEvent.Cells.push_back(GameId(modId, baseId));
                    }
                }
            }

            const auto* pCell = DataHandler::GetCellFromCoordinates(pDataHandler, pTES->centerGridX, pTES->centerGridY, pWorldSpace, 0);

            uint32_t baseId = 0;
            uint32_t modId = 0;
            if (m_world.GetModSystem().GetServerModId(pCell->formID, modId, baseId))
                changeEvent.PlayerCell = GameId(modId, baseId);

            changeEvent.CurrentGridX = pTES->centerGridX;
            changeEvent.CurrentGridY = pTES->centerGridY;

            m_dispatcher.trigger(changeEvent);
            m_worldSpaceId = worldSpaceId;
        }
    }

    else if (const auto pParentCell = pPlayer->GetParentCell())
    {
        m_worldSpaceId = 0;
        const auto cellId = pParentCell->formID;
        if (m_interiorCellId != cellId || aForceTrigger)
        {
            m_dispatcher.trigger(CellChangeEvent(cellId));
            m_interiorCellId = cellId;
        }
    }

    // exactly how the game does it too
    if (m_pLocation != pPlayer->locationForm)
    {
        m_dispatcher.trigger(LocationChangeEvent());
        m_pLocation = pPlayer->locationForm;
    }
}

void DiscoveryService::VisitForms() noexcept
{
    static Set<uint32_t> s_previousForms;
    s_previousForms = m_forms;

    const auto visitor = [this](TESObjectREFR* apReference)
    {
        const auto formId = apReference->formID;

        if (!m_forms.count(formId))
        {
            m_forms.insert(formId);

            m_dispatcher.enqueue(ReferenceAddedEvent(formId, apReference->formType));
        }
        else
            s_previousForms.erase(formId);

    };

    auto* const pActorHolder = ActorHolder::Get();
    if (!pActorHolder)
        return;

    for (uint32_t i = 0; i < pActorHolder->actorRefs.length; ++i)
    {
        auto* const pRefr = TESObjectREFR::GetByHandle(pActorHolder->actorRefs[i]);
        if (pRefr)
        {
            if (pRefr->GetNiNode())
            {
                visitor(pRefr);
            }
        }
    }

    // Not in actor holder
    visitor(PlayerCharacter::Get());

    // We dispatch removal events first to prevent needless reallocations
    for (auto formId : s_previousForms)
    {
        m_dispatcher.trigger(ReferenceRemovedEvent(formId));
        m_forms.erase(formId);
    }

    // Dispatch all adds
    m_dispatcher.update<ReferenceAddedEvent>();
}

void DiscoveryService::OnUpdate(const PreUpdateEvent& acUpdateEvent) noexcept
{
    TP_UNUSED(acUpdateEvent);

    VisitCell();
    VisitForms();
}

void DiscoveryService::OnConnected(const ConnectedEvent& acEvent) noexcept
{
    VisitCell(true);
}
