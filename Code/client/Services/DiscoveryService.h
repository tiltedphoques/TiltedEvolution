#pragma once

#include <Events/EventDispatcher.h>
#include <Games/Events.h>

#include <Structs/GridCellCoords.h>

struct World;

struct ConnectedEvent;
struct GameService;

struct PreUpdateEvent;
struct ActorSpawnedEvent;

struct TESObjectREFR;
struct TESWorldSpace;

/**
* Responsible for discovering loaded actors, dispatching cell changes,
* and notifying the rest of the client about newly discovered actors.
*/
struct DiscoveryService final
    : BSTEventSink<TESLoadGameEvent>
{
    DiscoveryService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~DiscoveryService() noexcept = default;

    TP_NOCOPYMOVE(DiscoveryService);

protected:
    /**
    * Checks whether the cell has changed, and if so,
    * notify the client about the new location.
    * @param aForceTrigger Forcefully notify the client about the current location
    */
    void VisitCell(bool aForceTrigger = false) noexcept;
    /**
    * Checks whether the grid cell has shifted, and if so,
    * notify the client about the newly loaded cells.
    * @param aWorldSpace The world space that the grid cell is in.
    * @param aNewCellGrid True if the player just walked into the new world space.
    */
    void DetectGridCellChange(TESWorldSpace* aWorldSpace, bool aNewCellGrid) noexcept;
    /**
    * @brief Dispatches (un)loaded actors.
    */
    void VisitForms() noexcept;

    /**
    * On each update, check whether the actor list or locational data has changed.
    * @param acPreUpdateEvent Update on PreUpdateEvent so that the actors are dispatched before anything else.
    */
    void OnUpdate(const PreUpdateEvent& acPreUpdateEvent) noexcept;

    /**
    * On connect, forcefully dispatch the loaded actors and locational data.
    */
    void OnConnected(const ConnectedEvent& acEvent) noexcept;

    /**
    * When the game finishes loading, forcefully dispatch the loaded actors and locational data.
    * This is a fix for the event that the player dies, and needs to rebroadcast its data.
    * TODO(cosideci): this system is incomplete.
    */
    BSTEventResult OnEvent(const TESLoadGameEvent*, const EventDispatcher<TESLoadGameEvent>*) override;

private:

    /**
    * Resets cached exterior cell data.
    * TODO(cosideci): confirm the usage of this function
    */
    void ResetCachedCellData() noexcept;
    /**
    * Checks whether the exterior cell grid has changed, and if so,
    * notify the client about the new location.
    * @param aForceTrigger Forcefully notify the client about the current location
    */
    void VisitExteriorCell(bool aForceTrigger) noexcept;
    /**
    * Checks whether the interior cell has changed, and if so,
    * notify the client about the new location.
    * @param aForceTrigger Forcefully notify the client about the current location
    */
    void VisitInteriorCell(bool aForceTrigger) noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;

    //! @brief Cached actor forms detected in the previous frame.
    Set<uint32_t> m_forms;
    /**
    * The center grid coordinates are the coordinates of the cell in the cell grid
    * where the cells around it in a 5 by 5 grid (by default) are loaded.
    */
    GridCellCoords m_centerGrid{};
    /**
    * The current grid coordinates are the coordinates of the cell that is currently
    * actually occupied by the player.
    */
    GridCellCoords m_currentGrid{};
    uint32_t m_worldSpaceId = 0;
    uint32_t m_interiorCellId = 0;
    struct TESForm *m_pLocation = nullptr;

    entt::scoped_connection m_preUpdateConnection;
    entt::scoped_connection m_connectedConnection;
};
