#pragma once

/**
* @brief Dispatched whenever an actor talks
*/
struct DialogueEvent
{
    DialogueEvent(uint32_t aActorID, String aVoiceFile)
        : ActorID(aActorID), VoiceFile(aVoiceFile)
    {}

    uint32_t ActorID;
    String VoiceFile;
};

