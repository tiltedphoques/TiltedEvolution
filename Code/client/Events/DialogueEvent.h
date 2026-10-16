#pragma once

/**
 * @brief Dispatched whenever an actor talks
 */
struct DialogueEvent
{
    DialogueEvent(uint32_t aActorID, String aVoiceFile, uint32_t aSceneID)
        : ActorID(aActorID)
        , VoiceFile(aVoiceFile)
        , SceneID(aSceneID)
    {
    }

    uint32_t ActorID;
    String VoiceFile;
    uint32_t SceneID; // Scene the line is spoken in, 0 for player dialogue menu lines and lines outside scenes
};
