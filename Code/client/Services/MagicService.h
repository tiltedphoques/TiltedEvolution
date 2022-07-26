#pragma once

#include <Games/Events.h>
#include <Events/EventDispatcher.h>
#include <Messages/AddTargetRequest.h>

struct World;
struct TransportService;

struct UpdateEvent;
struct SpellCastEvent;
struct InterruptCastEvent;
struct AddTargetEvent;

struct NotifySpellCast;
struct NotifyInterruptCast;
struct NotifyAddTarget;

/**
* @brief Handles magic spell casting and magic effects.
* 
* To the poor sod that thinks of venturing into this part of the code base..
* Yes, the magic sync code is a mess. It is easily the ugliest part of the codebase.
* I did not get around to refactoring this before going open source, but it definitely should be.
* I'm sorry, and good luck.
* 
* Contact cosideci for more info.
*/
struct MagicService
{
    MagicService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~MagicService() noexcept = default;

    TP_NOCOPYMOVE(MagicService);

protected:

    /**
    * @brief Checks to apply queued effects on each frame.
    */
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    /**
    * @brief Sends local spell cast to the server.
    */
    void OnSpellCastEvent(const SpellCastEvent& acSpellCastEvent) const noexcept;
    /**
    * @brief Casts a spell based on a server message.
    */
    void OnNotifySpellCast(const NotifySpellCast& acMessage) const noexcept;
    /**
    * @brief Sends local interruption of spell cast to the server.
    */
    void OnInterruptCastEvent(const InterruptCastEvent& acEvent) const noexcept;
    /**
    * @brief Interrupts a spell cast based on a server message.
    */
    void OnNotifyInterruptCast(const NotifyInterruptCast& acMessage) const noexcept;
    /**
    * @brief Sends magic effect and its target to the server.
    */
    void OnAddTargetEvent(const AddTargetEvent& acEvent) noexcept;
    /**
    * @brief Applies a magic effect based on a server message.
    */
    void OnNotifyAddTarget(const NotifyAddTarget& acMessage) const noexcept;

private:

    /**
    * Sometimes, certain magic effects are applied on actors that do not yet exist
    * within the client's entity component system (for example when a beast is summoned).
    * This function periodically checks whether the queued magic effects' targets exist already
    * and if so, sends out a message to add the magic effect to the target.
    */
    void ApplyQueuedEffects() noexcept;

    World& m_world;
    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;

    /**
    * @brief The queued magic effects.
    * @see ApplyQueuedEffects
    */
    Map<uint32_t, AddTargetRequest> m_queuedEffects;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_spellCastEventConnection;
    entt::scoped_connection m_notifySpellCastConnection;
    entt::scoped_connection m_interruptCastEventConnection;
    entt::scoped_connection m_notifyInterruptCastConnection;
    entt::scoped_connection m_addTargetEventConnection;
    entt::scoped_connection m_notifyAddTargetConnection;
};
