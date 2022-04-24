#pragma once

/**
* @brief Experience added to an arbitrary skill.
*/
struct AddExperienceEvent
{
    AddExperienceEvent(float aExperience) 
        : Experience(aExperience)
    {
    }

    float Experience{};
};
