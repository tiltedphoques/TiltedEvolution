#pragma once

struct QuestCallbackManager
{
    static QuestCallbackManager* Get() noexcept;

    void RegisterQuest(uint32_t formId);
    //void NotifyStage(uint32_t formid, uint16_t stage);
    //void ResetStage(uint32_t formid, uint16_t stage);
};
