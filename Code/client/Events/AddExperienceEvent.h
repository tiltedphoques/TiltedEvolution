#pragma once

struct AddExperienceEvent
{
    AddExperienceEvent(float aExperience) 
        : Experience(aExperience)
    {
    }

    float Experience{};
};
