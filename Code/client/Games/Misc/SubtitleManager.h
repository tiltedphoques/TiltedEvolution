#pragma once

struct TESObjectREFR;

struct SubtitleManager
{
    // TODO(cosideci): confirm usage of bool
    static SubtitleManager* Get() noexcept;

    void ShowSubtitle(TESObjectREFR* apSpeaker, const char* apSubtitleText, bool aIsInDialogue = false) noexcept;
    void* HideSubtitle(TESObjectREFR* apSpeaker) noexcept;
};
