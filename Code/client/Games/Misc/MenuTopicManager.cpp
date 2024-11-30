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
        for (auto option : *apThis->pOptions)
        {
            if (i == aIndex)
            {
                pText = option->text;
                break;
            }
            i++;
        }

        if (pText != nullptr)
            World::Get().GetRunner().Trigger(PlayerDialogueEvent(pText));
    }

    return TiltedPhoques::ThisCall(RealPlayDialogueOption, apThis, aIndex);
}

static TiltedPhoques::Initializer s_menuTopicHooks(
    []()
    {
        POINTER_SKYRIMSE(TPlayDialogueOption, s_playDialogueOption, 35269);

        RealPlayDialogueOption = s_playDialogueOption.Get();

        TP_HOOK(&RealPlayDialogueOption, HookPlayDialogueOption);
    });
