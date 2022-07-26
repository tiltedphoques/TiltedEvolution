#include <TiltedOnlinePCH.h>


#include <Games/Misc/QuestCallbackManager.h>

QuestCallbackManager* QuestCallbackManager::Get() noexcept
{
    POINTER_SKYRIMSE(QuestCallbackManager*, s_instance, 406735);
    POINTER_FALLOUT4(QuestCallbackManager*, s_instance, 232587);
    return *s_instance.Get();
}

void QuestCallbackManager::RegisterQuest(uint32_t formId)
{
    using TRegisterQuest = void(QuestCallbackManager*, uint32_t);
    POINTER_SKYRIMSE(TRegisterQuest, RegisterQuest, 53711);
    POINTER_FALLOUT4(TRegisterQuest, RegisterQuest, 1186742);

    RegisterQuest(this, formId);
}
