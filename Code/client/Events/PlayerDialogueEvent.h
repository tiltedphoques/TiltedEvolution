#pragma once

struct PlayerDialogueEvent
{
    PlayerDialogueEvent(String aText) 
        : Text(std::move(aText))
    {
    }

    String Text;
};
