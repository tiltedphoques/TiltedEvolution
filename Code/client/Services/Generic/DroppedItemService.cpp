#include <Services/DroppedItemService.h>
#include <Services/TransportService.h>

#include <World.h>

#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/ItemDroppedEvent.h>
#include <Events/DroppedItemPickedUpEvent.h>

#include <Messages/DropItemRequest.h>
#include <Messages/DropItemResponse.h>
#include <Messages/PickupDroppedItemRequest.h>
#include <Messages/PickupDroppedItemResponse.h>
#include <Messages/NotifyDroppedItems.h>
#include <Messages/NotifyDroppedItemRemoved.h>

#include <PlayerCharacter.h>
#include <Actor.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>
#include <Games/Overrides.h>

namespace
{
// How far a leftover local copy may be from the synced position to be reused instead of placing a new one.
constexpr float kAdoptionDistance = 128.f;
constexpr double kSpawnRetryInterval = 1.0;
// A dropped item falls and may roll away, wait for it to come to rest before telling others where it is.
constexpr double kMinSettleTime = 0.5;
constexpr double kMaxSettleTime = 3.0;
constexpr float kRestingMovement = 0.5f;
// How far a placed copy may end up from the synced position before it gets moved there.
constexpr float kPlacementTolerance = 32.f;


float DistanceSquared(const NiPoint3& acPosition, float aX, float aY, float aZ) noexcept
{
    const float dx = acPosition.x - aX;
    const float dy = acPosition.y - aY;
    const float dz = acPosition.z - aZ;
    return dx * dx + dy * dy + dz * dz;
}
} // namespace

DroppedItemService::DroppedItemService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&DroppedItemService::OnUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&DroppedItemService::OnDisconnected>(this);
    m_itemDroppedConnection = aDispatcher.sink<ItemDroppedEvent>().connect<&DroppedItemService::OnItemDropped>(this);
    m_pickedUpConnection = aDispatcher.sink<DroppedItemPickedUpEvent>().connect<&DroppedItemService::OnDroppedItemPickedUp>(this);
    m_dropItemResponseConnection = aDispatcher.sink<DropItemResponse>().connect<&DroppedItemService::OnDropItemResponse>(this);
    m_notifyDroppedItemsConnection = aDispatcher.sink<NotifyDroppedItems>().connect<&DroppedItemService::OnNotifyDroppedItems>(this);
    m_notifyDroppedItemRemovedConnection = aDispatcher.sink<NotifyDroppedItemRemoved>().connect<&DroppedItemService::OnNotifyDroppedItemRemoved>(this);
    m_pickupResponseConnection = aDispatcher.sink<PickupDroppedItemResponse>().connect<&DroppedItemService::OnPickupDroppedItemResponse>(this);
}

void DroppedItemService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    UpdateSettlingDrops(acEvent.Delta);

    if (m_unspawnedItems.empty())
        return;

    m_spawnTimer += acEvent.Delta;
    if (m_spawnTimer < kSpawnRetryInterval)
        return;

    m_spawnTimer = 0.0;
    SpawnPendingItems();
}

void DroppedItemService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    m_items.clear();
    m_settlingDrops.clear();
    m_pendingDrops.clear();
    m_unspawnedItems.clear();
    m_pendingPickups.clear();
    m_removedItems.clear();
}

void DroppedItemService::OnItemDropped(const ItemDroppedEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    // Quest items have their own handling through the quest leader.
    if (acEvent.Item.IsQuestItem || acEvent.Item.Count <= 0 || !acEvent.Item.BaseId)
        return;

    TESObjectREFR* pRef = Cast<TESObjectREFR>(TESForm::GetById(acEvent.RefId));
    if (!pRef || !pRef->baseForm || !pRef->parentCell)
        return;

    SettlingDrop drop{};
    drop.ActorId = acEvent.ActorId;
    drop.BaseFormId = pRef->baseForm->formID;
    drop.Item = acEvent.Item;
    drop.LastX = pRef->position.x;
    drop.LastY = pRef->position.y;
    drop.LastZ = pRef->position.z;

    m_settlingDrops[acEvent.RefId] = std::move(drop);
}

void DroppedItemService::UpdateSettlingDrops(double aDelta) noexcept
{
    for (auto it = m_settlingDrops.begin(); it != m_settlingDrops.end();)
    {
        const uint32_t refId = it->first;
        SettlingDrop& drop = it.value();

        TESObjectREFR* pRef = Cast<TESObjectREFR>(TESForm::GetById(refId));
        // Picked up again or otherwise gone before anyone else needed to know about it.
        if (!pRef || !pRef->baseForm || pRef->baseForm->formID != drop.BaseFormId || pRef->IsDeleted() || !pRef->parentCell)
        {
            it = m_settlingDrops.erase(it);
            continue;
        }

        drop.Elapsed += aDelta;

        const bool cIsResting = DistanceSquared(pRef->position, drop.LastX, drop.LastY, drop.LastZ) < kRestingMovement * kRestingMovement;
        drop.LastX = pRef->position.x;
        drop.LastY = pRef->position.y;
        drop.LastZ = pRef->position.z;

        if ((cIsResting && drop.Elapsed >= kMinSettleTime) || drop.Elapsed >= kMaxSettleTime)
        {
            SendDrop(refId, drop);
            it = m_settlingDrops.erase(it);
            continue;
        }

        ++it;
    }
}

void DroppedItemService::SendDrop(uint32_t aRefId, const SettlingDrop& acDrop) noexcept
{
    TESObjectREFR* pRef = Cast<TESObjectREFR>(TESForm::GetById(aRefId));
    if (!pRef || !pRef->parentCell)
        return;

    auto& modSystem = m_world.GetModSystem();

    DropItemRequest request{};
    request.RefId = aRefId;
    request.Data.Item = acDrop.Item;

    if (!modSystem.GetServerModId(pRef->parentCell->formID, request.Data.CellId))
    {
        spdlog::warn("Dropped item {:X} is in cell {:X} that has no server id", aRefId, pRef->parentCell->formID);
        return;
    }

    if (TESWorldSpace* pWorldSpace = pRef->GetWorldSpace())
        modSystem.GetServerModId(pWorldSpace->formID, request.Data.WorldSpaceId);

    request.Data.PositionX = pRef->position.x;
    request.Data.PositionY = pRef->position.y;
    request.Data.PositionZ = pRef->position.z;
    request.Data.RotationX = pRef->rotation.x;
    request.Data.RotationY = pRef->rotation.y;
    request.Data.RotationZ = pRef->rotation.z;

    m_pendingDrops[aRefId] = PendingDrop{acDrop.BaseFormId, acDrop.Item};

    spdlog::debug(
        "[DroppedItems] Actor {:X} dropped ref {:X} (base {:X}, count {}) in cell {:X} at ({}, {}, {}), settled after {:.2f}s", acDrop.ActorId, aRefId, acDrop.BaseFormId,
        acDrop.Item.Count, pRef->parentCell->formID, pRef->position.x, pRef->position.y, pRef->position.z, acDrop.Elapsed);

    m_transport.Send(request);
}

void DroppedItemService::OnDroppedItemPickedUp(const DroppedItemPickedUpEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    spdlog::debug("[DroppedItems] Actor {:X} picked up temporary ref {:X} (base {:X})", acEvent.ActorId, acEvent.RefId, acEvent.BaseFormId);

    // Picked up before it even landed, nobody else has seen it yet.
    if (auto settlingIt = m_settlingDrops.find(acEvent.RefId); settlingIt != m_settlingDrops.end() && settlingIt->second.BaseFormId == acEvent.BaseFormId)
    {
        m_settlingDrops.erase(settlingIt);
        return;
    }

    if (auto pendingIt = m_pendingDrops.find(acEvent.RefId); pendingIt != m_pendingDrops.end() && pendingIt->second.BaseFormId == acEvent.BaseFormId)
    {
        pendingIt.value().PickedUp = true;
        pendingIt.value().PickerId = acEvent.ActorId;
        return;
    }

    // The item was taken by someone else just before the local pickup went through.
    // Ask the server anyway, the rejection removes the copy this actor got.
    if (auto removedIt = m_removedItems.find(acEvent.RefId); removedIt != m_removedItems.end() && removedIt->second.second.BaseFormId == acEvent.BaseFormId)
    {
        const uint32_t serverId = removedIt->second.first;
        Inventory::Entry item = std::move(removedIt->second.second.Item);
        m_removedItems.erase(removedIt);

        SendPickup(serverId, acEvent.ActorId, std::move(item));
        return;
    }

    const auto it = std::find_if(
        m_items.begin(), m_items.end(), [&acEvent](const auto& acPair) { return acPair.second.RefId == acEvent.RefId && acPair.second.BaseFormId == acEvent.BaseFormId; });

    if (it == m_items.end())
        return;

    const uint32_t serverId = it->first;
    Inventory::Entry item = it->second.Item;
    m_items.erase(it);

    spdlog::debug("[DroppedItems] Ref {:X} is dropped item {:X}, asking the server for the pickup", acEvent.RefId, serverId);

    SendPickup(serverId, acEvent.ActorId, std::move(item));
}

void DroppedItemService::OnDropItemResponse(const DropItemResponse& acMessage) noexcept
{
    const auto it = m_pendingDrops.find(acMessage.RefId);
    if (it == m_pendingDrops.end())
        return;

    PendingDrop drop = std::move(it->second);
    m_pendingDrops.erase(it);

    spdlog::debug("[DroppedItems] Ref {:X} got dropped item id {:X}, picked up meanwhile: {}", acMessage.RefId, acMessage.ServerId, drop.PickedUp);

    // The server does not sync drops, the item stays local like in single player.
    if (acMessage.ServerId == 0)
        return;

    if (drop.PickedUp)
    {
        SendPickup(acMessage.ServerId, drop.PickerId, std::move(drop.Item));
        return;
    }

    m_items[acMessage.ServerId] = TrackedItem{acMessage.RefId, drop.BaseFormId, std::move(drop.Item)};
}

void DroppedItemService::OnNotifyDroppedItems(const NotifyDroppedItems& acMessage) noexcept
{
    for (const DroppedItemData& data : acMessage.Items)
    {
        spdlog::debug("[DroppedItems] Server sent dropped item {:X} (base {:X}:{:X}, count {})", data.ServerId, data.Item.BaseId.ModId, data.Item.BaseId.BaseId, data.Item.Count);

        // Already picked up locally, the server just has not processed that yet.
        if (m_pendingPickups.find(data.ServerId) != m_pendingPickups.end())
            continue;

        if (auto it = m_items.find(data.ServerId); it != m_items.end())
        {
            if (GetReference(it->second))
                continue;

            // The local copy is gone (e.g. the cell was reset), place it again.
            m_items.erase(it);
        }

        m_unspawnedItems[data.ServerId] = data;
    }

    SpawnPendingItems();
}

void DroppedItemService::OnNotifyDroppedItemRemoved(const NotifyDroppedItemRemoved& acMessage) noexcept
{
    m_unspawnedItems.erase(acMessage.ServerId);

    const auto it = m_items.find(acMessage.ServerId);
    spdlog::debug("[DroppedItems] Server removed dropped item {:X}, tracked locally: {}", acMessage.ServerId, it != m_items.end());
    if (it == m_items.end())
        return;

    if (TESObjectREFR* pRef = GetReference(it->second))
    {
        pRef->Disable();
        pRef->Delete();
    }

    // Only a short safety net for pickups that are still queued, so keep it small.
    if (m_removedItems.size() > 256)
        m_removedItems.clear();
    m_removedItems[it->second.RefId] = {acMessage.ServerId, std::move(it->second)};

    m_items.erase(it);
}

void DroppedItemService::OnPickupDroppedItemResponse(const PickupDroppedItemResponse& acMessage) noexcept
{
    const auto it = m_pendingPickups.find(acMessage.ServerId);
    if (it == m_pendingPickups.end())
        return;

    PendingPickup pickup = std::move(it->second);
    m_pendingPickups.erase(it);

    if (acMessage.Accepted)
        return;

    // Somebody else picked the item up first, so the copy this actor got must not exist.
    // This is not done under an inventory override, so the server and other players remove it as well.
    Actor* pActor = Cast<Actor>(TESForm::GetById(pickup.ActorId));
    if (!pActor)
        return;

    spdlog::info("Dropped item {:X} was already picked up by someone else, removing it from actor {:X}", acMessage.ServerId, pickup.ActorId);

    Inventory::Entry removal = std::move(pickup.Item);
    removal.Count = -removal.Count;
    pActor->AddOrRemoveItem(removal);
}

void DroppedItemService::SpawnPendingItems() noexcept
{
    for (auto it = m_unspawnedItems.begin(); it != m_unspawnedItems.end();)
    {
        if (SpawnItem(it->second))
            it = m_unspawnedItems.erase(it);
        else
            ++it;
    }
}

bool DroppedItemService::SpawnItem(const DroppedItemData& acData) noexcept
{
    auto& modSystem = m_world.GetModSystem();

    TESObjectCELL* pCell = Cast<TESObjectCELL>(TESForm::GetById(modSystem.GetGameId(acData.CellId)));
    if (!pCell || !pCell->IsAttached())
        return false;

    // Items are placed by passing them through the player's inventory, wait until that is safe.
    PlayerCharacter* pPlayer = PlayerCharacter::Get();
    if (!pPlayer || pPlayer->IsDead())
        return false;

    TESBoundObject* pObject = Cast<TESBoundObject>(TESForm::GetById(modSystem.GetGameId(acData.Item.BaseId)));
    if (!pObject)
    {
        spdlog::warn("Dropped item {:X} has unknown base object {:X}:{:X}", acData.ServerId, acData.Item.BaseId.ModId, acData.Item.BaseId.BaseId);
        return true;
    }

    const NiPoint3 position(glm::vec3(acData.PositionX, acData.PositionY, acData.PositionZ));
    const NiPoint3 rotation(glm::vec3(acData.RotationX, acData.RotationY, acData.RotationZ));

    // A copy from an earlier session may still be lying there (dropped items are saved), reuse it instead of doubling it.
    TESObjectREFR* pRef = FindUntrackedReference(pCell, pObject, position);
    if (!pRef)
        pRef = CreateReference(pCell, pObject, acData.Item, position, rotation);

    if (!pRef)
    {
        spdlog::error("Failed to place dropped item {:X}, base object {:X}", acData.ServerId, pObject->formID);
        return true;
    }

    m_items[acData.ServerId] = TrackedItem{pRef->formID, pObject->formID, acData.Item};

    spdlog::debug(
        "[DroppedItems] Placed dropped item {:X} as ref {:X} (base {:X}, count {}) at ({}, {}, {})", acData.ServerId, pRef->formID, pObject->formID, acData.Item.Count, pRef->position.x,
        pRef->position.y, pRef->position.z);

    return true;
}

TESObjectREFR* DroppedItemService::FindUntrackedReference(const TESObjectCELL* apCell, const TESBoundObject* apObject, const NiPoint3& acPosition) const noexcept
{
    const Vector<TESObjectREFR*> references = apCell->GetRefsByFormTypes({apObject->formType});

    for (TESObjectREFR* pRef : references)
    {
        if (pRef->baseForm != apObject || !pRef->IsTemporary() || pRef->IsDisabled() || pRef->IsDeleted())
            continue;

        const float dx = pRef->position.x - acPosition.x;
        const float dy = pRef->position.y - acPosition.y;
        const float dz = pRef->position.z - acPosition.z;
        if (dx * dx + dy * dy + dz * dz > kAdoptionDistance * kAdoptionDistance)
            continue;

        if (IsTrackedReference(pRef->formID))
            continue;

        spdlog::debug("[DroppedItems] Reusing existing local ref {:X} for a dropped item", pRef->formID);
        return pRef;
    }

    return nullptr;
}

TESObjectREFR* DroppedItemService::CreateReference(TESObjectCELL* apCell, TESBoundObject* apObject, const Inventory::Entry& acItem, NiPoint3 aPosition, NiPoint3 aRotation) const noexcept
{
    PlayerCharacter* pPlayer = PlayerCharacter::Get();
    if (!pPlayer)
        return nullptr;

    // The game only creates item references by dropping them from an inventory,
    // so the local player briefly holds the item and drops it at the synced position.
    // The override keeps both steps from being sent to the server as the player's own inventory changes.
    ScopedInventoryOverride _;

    ExtraDataList* pExtraData = TESObjectREFR::GetExtraDataFromItem(acItem);
    pPlayer->AddObjectToContainer(apObject, pExtraData, acItem.Count, nullptr);

    const BSPointerHandle<TESObjectREFR> handle = pPlayer->DropObject(apObject, pExtraData, acItem.Count, &aPosition, &aRotation);
    TESObjectREFR* pRef = handle ? TESObjectREFR::GetByHandle(handle.handle.iBits) : nullptr;

    if (!pRef)
    {
        pPlayer->RemoveItem(apObject, acItem.Count, ITEM_REMOVE_REASON::kRemove, pExtraData, nullptr);
        return nullptr;
    }

    // Moving a dropped item after the fact desyncs its physics (it floats and cannot be picked up),
    // so only do it when the item ended up in the wrong cell altogether.
    if (pRef->parentCell != apCell)
    {
        spdlog::debug("[DroppedItems] Ref {:X} was dropped into cell {:X} instead of {:X}, moving it", pRef->formID, pRef->parentCell ? pRef->parentCell->formID : 0, apCell->formID);
        pRef->MoveTo(apCell, aPosition);
    }
    else if (DistanceSquared(pRef->position, aPosition.x, aPosition.y, aPosition.z) > kPlacementTolerance * kPlacementTolerance)
    {
        spdlog::warn(
            "[DroppedItems] Ref {:X} was dropped at ({}, {}, {}) instead of ({}, {}, {})", pRef->formID, pRef->position.x, pRef->position.y, pRef->position.z, aPosition.x, aPosition.y,
            aPosition.z);
    }

    return pRef;
}

TESObjectREFR* DroppedItemService::GetReference(const TrackedItem& acItem) const noexcept
{
    TESObjectREFR* pRef = Cast<TESObjectREFR>(TESForm::GetById(acItem.RefId));

    // Temporary form ids get reused once a reference is gone, so make sure it is still the same item.
    if (!pRef || !pRef->baseForm || pRef->baseForm->formID != acItem.BaseFormId || pRef->IsDeleted())
        return nullptr;

    return pRef;
}

bool DroppedItemService::IsTrackedReference(uint32_t aRefId) const noexcept
{
    if (m_pendingDrops.find(aRefId) != m_pendingDrops.end() || m_settlingDrops.find(aRefId) != m_settlingDrops.end())
        return true;

    return std::any_of(m_items.begin(), m_items.end(), [aRefId](const auto& acPair) { return acPair.second.RefId == aRefId; });
}

void DroppedItemService::SendPickup(uint32_t aServerId, uint32_t aActorId, Inventory::Entry aItem) noexcept
{
    m_pendingPickups[aServerId] = PendingPickup{aActorId, std::move(aItem)};

    PickupDroppedItemRequest request{};
    request.ServerId = aServerId;
    m_transport.Send(request);
}
