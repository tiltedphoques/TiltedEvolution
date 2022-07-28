#pragma once

struct TESObjectREFR;

struct SubtitleManager
{
    static SubtitleManager* Get() noexcept;

    // TODO(cosideci): confirm usage of bool
    void ShowSubtitle(TESObjectREFR* apSpeaker, const char* apSubtitleText, TESTopicInfo* apTopicInfo, bool aIsInDialogue = false) noexcept;
    void* HideSubtitle(TESObjectREFR* apSpeaker) noexcept;
};
