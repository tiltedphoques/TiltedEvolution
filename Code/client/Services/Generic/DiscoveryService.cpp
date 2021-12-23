#include <TiltedOnlinePCH.h>

#include <Services/DiscoveryService.h>
#include <Games/TES.h>

#include <Games/References.h>

#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

#include <Events/ReferenceAddedEvent.h>
#include <Events/ReferenceRemovedEvent.h>
#include <Events/PreUpdateEvent.h>
#include <Events/GridCellChangeEvent.h>
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

#if TP_SKYRIM64
    EventDispatcherManager::Get()->loadGameEvent.RegisterSink(this);
#else
    GetEventDispatcher_TESLoadGameEvent()->RegisterSink(this);
#endif
}

void DiscoveryService::VisitCell(bool aForceTrigger) noexcept
{
    const PlayerCharacter* pPlayer = PlayerCharacter::Get();
    if (!pPlayer)
        return;

    if (pPlayer->GetWorldSpace())
    {
        VisitExteriorCell(aForceTrigger);
    }
    else if (pPlayer->GetParentCell())
    {
        VisitInteriorCell(aForceTrigger);
    }

    // exactly how the game does it too
    if (m_pLocation != pPlayer->locationForm)
    {
        m_dispatcher.trigger(LocationChangeEvent());
        m_pLocation = pPlayer->locationForm;
    }
}

void DiscoveryService::VisitExteriorCell(bool aForceTrigger) noexcept
{
    const auto pWorldSpace = PlayerCharacter::Get()->GetWorldSpace();

    m_interiorCellId = 0;
    const TES* pTES = TES::Get();
    const uint32_t worldSpaceId = pWorldSpace->formID;

    if (m_worldSpaceId != worldSpaceId || aForceTrigger)
    {
        DetectGridCellChange(pWorldSpace, true);
        aForceTrigger = true;
    }

    if (m_centerGridX != pTES->centerGridX || m_centerGridY != pTES->centerGridY)
        DetectGridCellChange(pWorldSpace, false);

    if (m_currentGridX != pTES->currentGridX || m_currentGridY != pTES->currentGridY || aForceTrigger)
    {
        CellChangeEvent cellChangeEvent;
        uint32_t baseId = 0;
        uint32_t modId = 0;

        if (m_world.GetModSystem().GetServerModId(pWorldSpace->formID, modId, baseId))
            cellChangeEvent.WorldSpaceId = GameId(modId, baseId);

        const TESObjectCELL* pCell = ModManager::Get()->GetCellFromCoordinates(pTES->currentGridX, pTES->currentGridY, pWorldSpace, 0);

        if (m_world.GetModSystem().GetServerModId(pCell->formID, modId, baseId))
            cellChangeEvent.CellId = GameId(modId, baseId);

        cellChangeEvent.CurrentCoords = GridCellCoords(pTES->currentGridX, pTES->currentGridY);

        m_dispatcher.trigger(cellChangeEvent);
        m_currentGridX = pTES->currentGridX;
        m_currentGridY = pTES->currentGridY;
    }
}

void DiscoveryService::VisitInteriorCell(bool aForceTrigger) noexcept
{
    ResetCachedCellData();

    const uint32_t cellId = PlayerCharacter::Get()->GetParentCell()->formID;
    if (m_interiorCellId != cellId || aForceTrigger)
    {
        CellChangeEvent cellChangeEvent;
        uint32_t baseId = 0;
        uint32_t modId = 0;

        if (m_world.GetModSystem().GetServerModId(cellId, modId, baseId))
            cellChangeEvent.CellId = GameId(modId, baseId);

        m_dispatcher.trigger(cellChangeEvent);
        m_interiorCellId = cellId;
    }
}

void DiscoveryService::DetectGridCellChange(TESWorldSpace* aWorldSpace, bool aNewGridCell) noexcept
{
    const TES* pTES = TES::Get();

    GridCellChangeEvent changeEvent;
    const uint32_t worldSpaceId = aWorldSpace->formID;
    changeEvent.WorldSpaceId = worldSpaceId;

    const int32_t startGridX = pTES->centerGridX - GridCellCoords::m_gridsToLoad / 2;
    const int32_t startGridY = pTES->centerGridY - GridCellCoords::m_gridsToLoad / 2;
    for (int32_t i = 0; i < GridCellCoords::m_gridsToLoad; ++i)
    {
        for (int32_t j = 0; j < GridCellCoords::m_gridsToLoad; ++j)
        {
            if (!aNewGridCell)
            {
                if (GridCellCoords::IsCellInGridCell(GridCellCoords(m_centerGridX, m_centerGridY),
                                                     GridCellCoords(startGridX + i, startGridY + j)))
                {
                    continue;
                }
            }

            const TESObjectCELL* pCell = ModManager::Get()->GetCellFromCoordinates(startGridX + i, startGridY + j, aWorldSpace, 0);

            if (!pCell)
            {
                spdlog::warn("Cell not found at coordinates ({}, {}) in worldspace {:X}", startGridX + i,
                             startGridY + j, aWorldSpace->formID);
                continue;
            }

            uint32_t baseId = 0;
            uint32_t modId = 0;
            if (m_world.GetModSystem().GetServerModId(pCell->formID, modId, baseId))
                changeEvent.Cells.push_back(GameId(modId, baseId));
        }
    }

    const TESObjectCELL* pCell = ModManager::Get()->GetCellFromCoordinates(pTES->centerGridX, pTES->centerGridY, aWorldSpace, 0);

    uint32_t baseId = 0;
    uint32_t modId = 0;
    if (m_world.GetModSystem().GetServerModId(pCell->formID, modId, baseId))
        changeEvent.PlayerCell = GameId(modId, baseId);

    m_centerGridX = pTES->centerGridX;
    m_centerGridY = pTES->centerGridY;

    changeEvent.CenterCoords = GridCellCoords(pTES->centerGridX, pTES->centerGridY);
    changeEvent.PlayerCoords = GridCellCoords(pTES->currentGridX, pTES->currentGridY);

    m_dispatcher.trigger(changeEvent);
    m_worldSpaceId = worldSpaceId;
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

    ProcessLists* const pProcessLists = ProcessLists::Get();
    if (!pProcessLists)
        return;

    for (uint32_t i = 0; i < pProcessLists->HighActorHandleArray.length; ++i)
    {
        TESObjectREFR* const pRefr = TESObjectREFR::GetByHandle(pProcessLists->HighActorHandleArray[i]);
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
    for (uint32_t formId : s_previousForms)
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

BSTEventResult DiscoveryService::OnEvent(const TESLoadGameEvent*, const EventDispatcher<TESLoadGameEvent>*)
{
    spdlog::info("Finished loading, triggering visit cell");
    VisitCell(true);
    return BSTEventResult::kOk;
}

void DiscoveryService::ResetCachedCellData() noexcept
{
    m_worldSpaceId = 0;
    m_centerGridX = 0x7FFFFFFF;
    m_centerGridY = 0x7FFFFFFF;
    m_currentGridX = 0x7FFFFFFF;
    m_currentGridY = 0x7FFFFFFF;
}

