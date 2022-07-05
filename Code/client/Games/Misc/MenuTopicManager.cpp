#include "MenuTopicManager.h"

#include <Events/PlayerDialogueEvent.h>

TP_THIS_FUNCTION(TPlayDialogueOption, bool, MenuTopicManager, int32_t aIndex);
static TPlayDialogueOption* RealPlayDialogueOption = nullptr;

MenuTopicManager* MenuTopicManager::Get() noexcept
{
    POINTER_SKYRIMSE(MenuTopicManager*, s_singleton, 401099);

    return *s_singleton.Get();
}

bool TP_MAKE_THISCALL(HookPlayDialogueOption, MenuTopicManager, int32_t aIndex)
{
    if (apThis->pOptions)
    {
        int i = 0;
        const char* pText = nullptr;
        MenuTopicManager::DialogueOption* pOption = nullptr;
        for (auto option : *apThis->pOptions)
        {
            if (i == aIndex)
            {
                pText = option->text;
                pOption = option;
                break;
            }
            i++;
        }

        auto* pSpeaker = TESObjectREFR::GetByHandle(apThis->hSpeaker);
        spdlog::critical("Speaker: {:X}, topic: {:X}", pSpeaker->formID, pOption->pTopic->formID);

        if (pText != nullptr)
            World::Get().GetRunner().Trigger(PlayerDialogueEvent(pText));
    }

    return ThisCall(RealPlayDialogueOption, apThis, aIndex);
}

TiltedPhoques::Initializer s_menuTopicHooks([]()
{
    POINTER_SKYRIMSE(TPlayDialogueOption, s_playDialogueOption, 35269);

    RealPlayDialogueOption = s_playDialogueOption.Get();

    TP_HOOK(&RealPlayDialogueOption, HookPlayDialogueOption);
});


