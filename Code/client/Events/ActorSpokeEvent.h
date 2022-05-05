#pragma once

/**
* @brief Dispatched whenever an actor talks
*/
struct ActorSpokeEvent
{
    ActorSpokeEvent(uint32_t aActorID, String aVoiceFile)
        : ActorID(aActorID), VoiceFile(aVoiceFile)
    {}

    uint32_t ActorID;
    String VoiceFile;
};

