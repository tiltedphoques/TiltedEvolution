#pragma once

struct MenuTopicManager
{
    static MenuTopicManager* Get() noexcept;

    struct DialogueOption
    {
        const char* text;
        uint8_t pad8[0x50 - 0x8];
        TESForm* pTopic;
    };

    uint8_t unk0[0x20];
    GameList<DialogueOption>* pOptions;
    uint8_t unk28[0x68 - 0x28];
    uint32_t hSpeaker;
};
