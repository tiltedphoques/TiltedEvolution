#include <Services/DroppedItemService.h>

#include <World.h>

#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/DropItemEvent.h>
#include <Events/ReferencePickUpEvent.h>

#include <Messages/DropItemRequest.h>
#include <Messages/DropItemResponse.h>
#include <Messages/DroppedItemMoveRequest.h>
#include <Messages/PickUpDroppedItemRequest.h>
#include <Messages/NotifyDroppedItemsSpawn.h>
#include <Messages/NotifyDroppedItemMove.h>
#include <Messages/NotifyDroppedItemsRemove.h>

#include <Games/TES.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>
#include <Forms/TESBoundObject.h>

namespace
{
// How often the simulating client sends the transform while the item moves.
constexpr double cSendInterval = 0.1;
// The item counts as settled once it moves slower than this (units per second)...
constexpr float cRestSpeed = 2.f;
// ...for this long. Havok puts resting bodies to sleep, so their velocity drops to zero.
constexpr double cRestDuration = 0.5;
// Stops the simulation of an item that never settles, such as one jittering against geometry.
constexpr double cMaxSimulationTime = 10.0;
// A following copy gives up and rests where it is if the stream stops.
constexpr double cFollowTimeout = 3.0;
// Time a following copy gets to reach its final transform.
constexpr double cSettleTimeout = 1.0;
constexpr float cSettleDistance = 1.f;
constexpr float cMinTranslationSpeed = 10.f;

DroppedItemTransform GetTransform(const TESObjectREFR* apReference) noexcept
{
    DroppedItemTransform transform{};
    transform.Position = apReference->position;
    transform.Rotation = apReference->rotation;
    return transform;
}
} // namespace

DroppedItemService::DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&DroppedItemService::OnUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&DroppedItemService::OnDisconnected>(this);
    m_dropItemConnection = aDispatcher.sink<DropItemEvent>().connect<&DroppedItemService::OnDropItem>(this);
    m_referencePickUpConnection = aDispatcher.sink<ReferencePickUpEvent>().connect<&DroppedItemService::OnReferencePickUp>(this);
    m_dropItemResponseConnection = aDispatcher.sink<DropItemResponse>().connect<&DroppedItemService::OnDropItemResponse>(this);
    m_droppedItemsSpawnConnection = aDispatcher.sink<NotifyDroppedItemsSpawn>().connect<&DroppedItemService::OnNotifyDroppedItemsSpawn>(this);
    m_droppedItemMoveConnection = aDispatcher.sink<NotifyDroppedItemMove>().connect<&DroppedItemService::OnNotifyDroppedItemMove>(this);
    m_droppedItemsRemoveConnection = aDispatcher.sink<NotifyDroppedItemsRemove>().connect<&DroppedItemService::OnNotifyDroppedItemsRemove>(this);
}

void DroppedItemService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    for (auto& item : m_items)
    {
        if (item.IsAtRest)
            continue;

        if (item.IsSimulatedLocally)
            UpdateSimulatedItem(item, acEvent.Delta);
        else
            UpdateFollowingItem(item, acEvent.Delta);
    }
}

void DroppedItemService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // The server owns registered items. Keeping a copy would leave an untracked item behind.
    for (const auto& item : m_items)
    {
        if (item.ServerId != 0)
            DeleteReference(item);
    }

    m_items.clear();
}

void DroppedItemService::OnDropItem(const DropItemEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected() || !m_world.GetServerSettings().ItemDropsEnabled)
        return;

    auto* pReference = Cast<TESObjectREFR>(TESForm::GetById(acEvent.FormId));
    if (!pReference || !pReference->baseForm)
        return;

    TESObjectCELL* pCell = pReference->GetParentCellEx();
    if (!pCell)
        return;

    auto& modSystem = m_world.GetModSystem();

    DropItemRequest request{};
    request.LocalId = acEvent.FormId;
    request.Item = acEvent.Item;
    request.Transform = GetTransform(pReference);

    if (!modSystem.GetServerModId(pCell->formID, request.CellId))
    {
        spdlog::error("{}: server cell id not found for cell {:X}", __FUNCTION__, pCell->formID);
        return;
    }

    if (TESWorldSpace* pWorldSpace = pReference->GetWorldSpace())
    {
        if (!modSystem.GetServerModId(pWorldSpace->formID, request.WorldSpaceId))
        {
            spdlog::error("{}: server worldspace id not found for worldspace {:X}", __FUNCTION__, pWorldSpace->formID);
            return;
        }
    }

    TrackedItem item{};
    item.FormId = acEvent.FormId;
    item.BaseFormId = pReference->baseForm->formID;
    item.IsSimulatedLocally = true;
    item.LastTransform = request.Transform;
    item.LastSentTransform = request.Transform;
    m_items.push_back(item);

    m_transport.Send(request);
}

void DroppedItemService::OnReferencePickUp(const ReferencePickUpEvent& acEvent) noexcept
{
    TrackedItem* pItem = FindByFormId(acEvent.FormId);
    if (!pItem)
        return;

    if (pItem->ServerId == 0)
    {
        // The server has not answered the drop yet; send the pickup once it does.
        pItem->IsPickedUpBeforeRegistration = true;
        pItem->IsAtRest = true;
        return;
    }

    SendPickUp(*pItem);
    Erase(*pItem);
}

void DroppedItemService::OnDropItemResponse(const DropItemResponse& acMessage) noexcept
{
    // Copies spawned for other players' drops always have a server id, so this only matches the local player's drops.
    TrackedItem* pItem = FindByFormId(acMessage.LocalId);
    if (!pItem || pItem->ServerId != 0)
        return;

    // The server did not register the drop, so the item stays local only.
    if (acMessage.ServerId == 0)
    {
        Erase(*pItem);
        return;
    }

    pItem->ServerId = acMessage.ServerId;

    if (pItem->IsPickedUpBeforeRegistration)
    {
        SendPickUp(*pItem);
        Erase(*pItem);
        return;
    }

    // The item settled before the server answered.
    if (pItem->IsAtRest)
        SendMove(*pItem);
}

void DroppedItemService::OnNotifyDroppedItemsSpawn(const NotifyDroppedItemsSpawn& acMessage) noexcept
{
    m_items.reserve(m_items.size() + acMessage.Items.size());

    for (const DroppedItemData& data : acMessage.Items)
    {
        if (FindByServerId(data.ServerId))
            continue;

        TESObjectREFR* pReference = SpawnItem(data);
        if (!pReference)
        {
            spdlog::warn("{}: failed to spawn dropped item {:X} ({:X}:{:X})", __FUNCTION__, data.ServerId, data.Item.BaseId.ModId, data.Item.BaseId.BaseId);
            continue;
        }

        TrackedItem item{};
        item.FormId = pReference->formID;
        item.BaseFormId = pReference->baseForm->formID;
        item.ServerId = data.ServerId;
        item.IsAtRest = data.IsAtRest;
        item.TargetTransform = data.Transform;

        // Physics stays with the simulating client while the item moves.
        if (!item.IsAtRest)
            EnsureKeyframed(item, pReference);

        m_items.push_back(item);
    }
}

void DroppedItemService::OnNotifyDroppedItemMove(const NotifyDroppedItemMove& acMessage) noexcept
{
    const auto& move = acMessage.Move;

    TrackedItem* pItem = FindByServerId(move.ServerId);
    if (!pItem || pItem->IsSimulatedLocally)
        return;

    // An item that gave up after cFollowTimeout resumes following, so a late update still lands it in the right spot.
    pItem->IsAtRest = false;
    pItem->TimeSinceUpdate = 0.0;
    pItem->TargetTransform = move.Transform;
    pItem->IsSettling = move.IsAtRest;

    TESObjectREFR* pReference = GetReference(*pItem);

    const NiPoint3 position(move.Transform.Position);
    const NiPoint3 rotation(move.Transform.Rotation);

    // Without 3D there is nothing to animate; place the reference so it loads in the right spot.
    if (!pReference || !pReference->GetNiNode())
    {
        if (pReference)
        {
            pReference->SetLocation(position);
            pReference->SetRotation(rotation.x, rotation.y, rotation.z);
        }

        pItem->IsAtRest = move.IsAtRest;
        return;
    }

    EnsureKeyframed(*pItem, pReference);

    // Reach the new sample by the time the next one arrives.
    const float distance = glm::distance(static_cast<glm::vec3>(pReference->position), static_cast<glm::vec3>(position));
    const float speed = std::max(distance / static_cast<float>(cSendInterval), cMinTranslationSpeed);
    pReference->TranslateTo(position, rotation, speed);
}

void DroppedItemService::OnNotifyDroppedItemsRemove(const NotifyDroppedItemsRemove& acMessage) noexcept
{
    for (const uint32_t serverId : acMessage.ServerIds)
    {
        TrackedItem* pItem = FindByServerId(serverId);
        if (!pItem)
            continue;

        DeleteReference(*pItem);
        Erase(*pItem);
    }
}

void DroppedItemService::UpdateSimulatedItem(TrackedItem& aItem, double aDelta) noexcept
{
    if (TESObjectREFR* pReference = GetReference(aItem))
    {
        aItem.SimulationTime += aDelta;
        aItem.TimeSinceSend += aDelta;
        aItem.LastTransform = GetTransform(pReference);

        // Without 3D there is no rigid body yet; keep waiting then.
        if (pReference->GetNiNode())
        {
            NiPoint3 velocity{};
            pReference->GetLinearVelocity(velocity);

            if (glm::length(static_cast<glm::vec3>(velocity)) < cRestSpeed)
                aItem.RestTime += aDelta;
            else
                aItem.RestTime = 0.0;
        }

        aItem.IsAtRest = aItem.RestTime >= cRestDuration || aItem.SimulationTime >= cMaxSimulationTime;
    }
    else
    {
        // The reference is gone, e.g. its cell unloaded. Leave the item where it was last seen.
        aItem.IsAtRest = true;
    }

    // Without a server id yet, the drop response sends the final transform.
    if (aItem.ServerId == 0)
        return;

    if (aItem.IsAtRest || (aItem.TimeSinceSend >= cSendInterval && aItem.LastTransform != aItem.LastSentTransform))
        SendMove(aItem);
}

void DroppedItemService::UpdateFollowingItem(TrackedItem& aItem, double aDelta) noexcept
{
    aItem.TimeSinceUpdate += aDelta;

    TESObjectREFR* pReference = GetReference(aItem);
    if (!pReference)
        return;

    // The copy may have spawned before its 3D loaded.
    EnsureKeyframed(aItem, pReference);

    if (aItem.IsSettling)
    {
        const float distance = glm::distance(static_cast<glm::vec3>(pReference->position), static_cast<glm::vec3>(aItem.TargetTransform.Position));
        if (distance <= cSettleDistance || aItem.TimeSinceUpdate >= cSettleTimeout)
            FinishFollowing(aItem);
    }
    else if (aItem.TimeSinceUpdate >= cFollowTimeout)
    {
        spdlog::debug("{}: no transform for dropped item {:X} in {}s, letting it rest", __FUNCTION__, aItem.ServerId, cFollowTimeout);
        FinishFollowing(aItem);
    }
}

void DroppedItemService::SendMove(TrackedItem& aItem) noexcept
{
    DroppedItemMoveRequest request{};
    request.Move.ServerId = aItem.ServerId;
    request.Move.Transform = aItem.LastTransform;
    request.Move.IsAtRest = aItem.IsAtRest;

    m_transport.Send(request);

    aItem.TimeSinceSend = 0.0;
    aItem.LastSentTransform = aItem.LastTransform;
}

void DroppedItemService::SendPickUp(const TrackedItem& acItem) const noexcept
{
    PickUpDroppedItemRequest request{};
    request.ServerId = acItem.ServerId;

    m_transport.Send(request);
}

void DroppedItemService::EnsureKeyframed(TrackedItem& aItem, TESObjectREFR* apReference) noexcept
{
    if (!aItem.IsKeyframed)
        aItem.IsKeyframed = apReference->SetMotionType(TESObjectREFR::MotionType::kKeyframed);
}

void DroppedItemService::FinishFollowing(TrackedItem& aItem) noexcept
{
    aItem.IsAtRest = true;
    aItem.IsSettling = false;

    TESObjectREFR* pReference = GetReference(aItem);
    if (!pReference)
        return;

    pReference->StopTranslation();

    // Give physics back, so the item reacts to hits like any other loose item.
    if (aItem.IsKeyframed)
        pReference->SetMotionType(TESObjectREFR::MotionType::kDynamic);

    aItem.IsKeyframed = false;
}

TESObjectREFR* DroppedItemService::SpawnItem(const DroppedItemData& acData) const noexcept
{
    auto& modSystem = m_world.GetModSystem();

    auto* pBaseForm = Cast<TESBoundObject>(TESForm::GetById(modSystem.GetGameId(acData.Item.BaseId)));
    if (!pBaseForm)
        return nullptr;

    TESObjectCELL* pCell = nullptr;
    TESWorldSpace* pWorldSpace = nullptr;

    // For an exterior, the engine picks the cell from the position.
    if (acData.WorldSpaceId)
        pWorldSpace = Cast<TESWorldSpace>(TESForm::GetById(modSystem.GetGameId(acData.WorldSpaceId)));
    else
        pCell = Cast<TESObjectCELL>(TESForm::GetById(modSystem.GetGameId(acData.CellId)));

    if (!pCell && !pWorldSpace)
        return nullptr;

    NiPoint3 position(acData.Transform.Position);
    NiPoint3 rotation(acData.Transform.Rotation);

    TESObjectREFR* pReference = ModManager::Get()->SpawnReference(pBaseForm, position, rotation, pCell, pWorldSpace);
    if (!pReference)
        return nullptr;

    pReference->SetItemData(acData.Item);

    return pReference;
}

TESObjectREFR* DroppedItemService::GetReference(const TrackedItem& acItem) const noexcept
{
    auto* pReference = Cast<TESObjectREFR>(TESForm::GetById(acItem.FormId));
    if (!pReference || !pReference->baseForm || pReference->baseForm->formID != acItem.BaseFormId)
        return nullptr;

    return pReference;
}

void DroppedItemService::DeleteReference(const TrackedItem& acItem) const noexcept
{
    TESObjectREFR* pReference = GetReference(acItem);
    if (!pReference)
        return;

    pReference->Disable();
    pReference->Delete();
}

DroppedItemService::TrackedItem* DroppedItemService::FindByFormId(uint32_t aFormId) noexcept
{
    const auto it = std::find_if(m_items.begin(), m_items.end(), [aFormId](const TrackedItem& acItem) { return acItem.FormId == aFormId; });
    return it != m_items.end() ? &*it : nullptr;
}

DroppedItemService::TrackedItem* DroppedItemService::FindByServerId(uint32_t aServerId) noexcept
{
    if (aServerId == 0)
        return nullptr;

    const auto it = std::find_if(m_items.begin(), m_items.end(), [aServerId](const TrackedItem& acItem) { return acItem.ServerId == aServerId; });
    return it != m_items.end() ? &*it : nullptr;
}

// The order of the items does not matter, so swap with the last one instead of shifting the rest.
void DroppedItemService::Erase(const TrackedItem& acItem) noexcept
{
    auto& item = m_items[&acItem - m_items.data()];
    if (&item != &m_items.back())
        item = std::move(m_items.back());

    m_items.pop_back();
}
