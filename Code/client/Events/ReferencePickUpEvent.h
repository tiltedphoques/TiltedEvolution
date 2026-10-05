#pragma once

/**
 * @brief Dispatched when a local actor picks up a temporary reference, such as a dropped item.
 */
struct ReferencePickUpEvent
{
    explicit ReferencePickUpEvent(const uint32_t aFormId)
        : FormId(aFormId)
    {
    }

    uint32_t FormId{};
};
