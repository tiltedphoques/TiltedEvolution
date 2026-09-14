#pragma once

/**
 * @brief Dispatched when an object animation has been triggered.
 */
struct ScriptAnimationEvent
{
    ScriptAnimationEvent(uint32_t aFormID, String aAnimation, String aEventName)
        : FormID(aFormID)
        , Animation(aAnimation)
        , EventName(aEventName)
    {
    }

    // Local form id of the animating reference; ObjectService::OnScriptAnimationEvent
    // translates it to a server id before it goes on the wire.
    uint32_t FormID;
    String Animation;
    String EventName;
};
