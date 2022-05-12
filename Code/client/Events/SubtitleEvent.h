#pragma once

struct SubtitleEvent
{
    SubtitleEvent(uint32_t aSpeakerID, String aText)
        : SpeakerID(aSpeakerID), Text(aText)
    {
    }

    uint32_t SpeakerID;
    String Text;
};
