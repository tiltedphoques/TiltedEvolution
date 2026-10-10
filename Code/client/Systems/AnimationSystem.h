#pragma once

#include <Components.h>

struct World;
struct Actor;
struct Movement;
struct RequestActionEvents;

/**
 * @brief Applies animations coming from remote actors.
 */
struct AnimationSystem
{
    /**
     * @brief Ran periodically to check for new animations to apply.
     * @param aWorld The registry where the actor in question lives.
     * @param apActor The actor to-be-updated.
     * @param aAnimationComponent The animation component attached to the actor.
     * @param aTick The owner tick currently being played back, actions up to it are run.
     */
    static void Update(World& aWorld, Actor* apActor, RemoteAnimationComponent& aAnimationComponent, uint64_t aTick) noexcept;
    /**
     * @brief Sets up the animation system for a particular actor.
     * @param aWorld The registry where the actor in question lives.
     * @param aEntity The entity attached to the actor.
     */
    static void Setup(World& aWorld, entt::entity aEntity) noexcept;
    /**
     * @brief Unregisters an actor from receiving remote animations.
     *
     * This function is not being used.
     *
     * @param aWorld The registry where the actor in question lives.
     * @param aEntity The entity attached to the actor.
     */
    static void Clean(World& aWorld, entt::entity aEntity) noexcept;
    /**
     * @brief Adds multiple actions to be replayed.
     * @param aAnimationComponent The animation component attached to the actor in question.
     * @param acReplay The replay data.
     */
    static void AddActionsForReplay(RemoteAnimationComponent& aAnimationComponent, const ActionReplayChain& acReplay) noexcept;
    /**
     * @brief Adds an action (animation) to be processed.
     * @param aAnimationComponent The animation component attached to the actor in question.
     * @param acActionDiff The differential data of the animation.
     */
    static void AddAction(RemoteAnimationComponent& aAnimationComponent, const std::string& acActionDiff) noexcept;
    /**
     * @brief Captures the movement snapshot of a local actor.
     * @param aWorld The registry where the actor in question lives.
     * @param apActor The local actor.
     * @param aMovement The output movement snapshot.
     */
    static void SerializeMovement(World& aWorld, Actor* apActor, Movement& aMovement) noexcept;
    /**
     * @brief Moves the actions a local actor performed since the last call into the message to-be-sent.
     * @param aMessage The output message.
     * @param localComponent The local component of the actor, gives the server id and tracks the latest action.
     * @param animationComponent The local animation component of the actor holding the pending actions.
     */
    static void SerializeActions(RequestActionEvents& aMessage, LocalComponent& localComponent, LocalAnimationComponent& animationComponent) noexcept;
    /**
     * @brief Serializes the actions to-be-sent.
     *
     * This function is not being used.
     */
    static bool Serialize(World& aWorld, const ActionEvent& aActionEvent, const ActionEvent& aLastProcessedAction, std::string* apData);
};
