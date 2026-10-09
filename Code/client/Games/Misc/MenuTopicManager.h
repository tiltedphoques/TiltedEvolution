#pragma once

struct TESObjectREFR;

struct MenuTopicManager
{
    static MenuTopicManager* Get() noexcept;
    static bool IsPlayerDialogueSpeaker(const TESObjectREFR* apSpeaker) noexcept;

    bool IsCurrentSpeaker(const BSPointerHandle<TESObjectREFR>& acHandle) const noexcept
    {
        return menuOpen && acHandle && speaker.handle.iBits == acHandle.handle.iBits;
    }

#ifdef TP_FALLOUT4
    uint8_t pad0[0x14];
    BSPointerHandle<TESObjectREFR> speaker;
    uint8_t pad18[0x4C - 0x18];
    bool menuOpen;
#else
    struct DialogueOption
    {
        const char* text;
    };

    uint8_t pad0[0x20];
    GameList<DialogueOption>* pOptions;

    uint8_t pad28[0x68 - 0x28];
    BSPointerHandle<TESObjectREFR> speaker;
    uint8_t pad6C[0xB1 - 0x6C];
    bool menuOpen;
#endif
};

#ifdef TP_FALLOUT4
static_assert(offsetof(MenuTopicManager, speaker) == 0x14);
static_assert(offsetof(MenuTopicManager, menuOpen) == 0x4C);
#else
static_assert(offsetof(MenuTopicManager, pOptions) == 0x20);
static_assert(offsetof(MenuTopicManager, speaker) == 0x68);
static_assert(offsetof(MenuTopicManager, menuOpen) == 0xB1);

#endif
