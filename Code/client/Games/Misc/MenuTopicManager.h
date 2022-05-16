#pragma once

struct MenuTopicManager
{
    static MenuTopicManager* Get() noexcept;

    struct DialogueOption
    {
        const char* text;
    };

    uint8_t pad0[0x20];
    GameList<DialogueOption>* pOptions;
};
