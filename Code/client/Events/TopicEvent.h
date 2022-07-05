#pragma once

/**
* @brief Triggers when a topic script event is dispatched.
*/
struct TopicEvent
{
    TopicEvent(uint32_t aSpeakerID, uint32_t aTopicID1, uint32_t aType, uint32_t aTopicID2) 
        : SpeakerID(aSpeakerID), TopicID1(aTopicID1), Type(aType), TopicID2(aTopicID2)
    {
    }

    uint32_t SpeakerID;
    uint32_t TopicID1;
    uint32_t Type;
    uint32_t TopicID2;
};
