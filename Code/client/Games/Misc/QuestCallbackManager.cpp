#include <TiltedOnlinePCH.h>


#include <Games/Misc/QuestCallbackManager.h>

QuestCallbackManager* QuestCallbackManager::Get() noexcept
{
    POINTER_SKYRIMSE(QuestCallbackManager*, s_instance, 0x142FE9FB0 - 0x140000000);
    POINTER_FALLOUT4(QuestCallbackManager*, s_instance, 0x145ADCFF0 - 0x140000000);
    return *s_instance.Get();
}

void QuestCallbackManager::RegisterQuest(uint32_t formId)
{
    using TRegisterQuest = void(QuestCallbackManager*, uint32_t);
    POINTER_SKYRIMSE(TRegisterQuest, RegisterQuest, 0x14093F270 - 0x140000000);
    POINTER_FALLOUT4(TRegisterQuest, RegisterQuest, 0x14134B0D0 - 0x140000000);

    RegisterQuest(this, formId);
}
