#include <Services/DiscoveryService.h>
#include <Games/TES.h>

#include <Games/References.h>

#include <Games/Skyrim/Forms/TESObjectCELL.h>
#include <Games/Fallout4/Forms/TESObjectCELL.h>

#include <Events/ReferenceAddedEvent.h>
#include <Events/ReferenceRemovedEvent.h>
#include <Events/PreUpdateEvent.h>
#include <Events/CellChangeEvent.h>
#include <Events/LocationChangeEvent.h>
#include <Events/ConnectedEvent.h>


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

    const auto cellId = pPlayer->GetCellId();
    if(m_cellId != cellId || aForceTrigger)
    {
        m_dispatcher.trigger(CellChangeEvent(cellId));
        m_cellId = cellId;
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

    const auto pTES = TES::Get();
    if (!pTES)
        return;

    const auto pCells = pTES->cells;
    if (!pCells)
        return;

    const auto cellCount = pCells->dimension * pCells->dimension;
    for (uint32_t i = 0; i < cellCount + 1; ++i)
    {
        const auto pCell = i == cellCount ? pTES->interiorCell : pCells->arr[i];

        if (!pCell || !pCell->IsValid())
            continue;

#if TP_SKYRIM64
        auto pCurrent = pCell->refData.refArray;

        for (auto j = 0; j < pCell->refData.Count();)
        {
            auto pRef = pCurrent->Get();
            if (pRef)
            {
                if (pRef->GetNiNode())
                {
                    visitor(pRef);
                }
                ++j;
            }

            ++pCurrent;
        }
#else

        for (uint32_t j = 0; j < pCell->objects.length; ++j)
        {
            auto pObject = pCell->objects[j];

            if (!pObject)
                continue;

            if (pObject->GetNiNode())
            {
                visitor(pObject);
            }
        }

#endif
    }

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
