#include "SubtitleManager.h"

#include <Events/SubtitleEvent.h>

#include <TESObjectREFR.h>
#include <Games/ActorExtension.h>

// TODO: ft
SubtitleManager* SubtitleManager::Get() noexcept
{
#if TP_SKYRIM64
    POINTER_SKYRIMSE(SubtitleManager*, s_singleton, 400443);
    return *s_singleton.Get();
#else
    return nullptr;
#endif
}

TP_THIS_FUNCTION(TShowSubtitle, void, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText,
                 bool aIsInDialogue);
static TShowSubtitle* RealShowSubtitle = nullptr;

void SubtitleManager::ShowSubtitle(TESObjectREFR* apSpeaker, const char* apSubtitleText, bool aUnk1) noexcept
{
    ThisCall(RealShowSubtitle, this, apSpeaker, apSubtitleText, aUnk1);
}

// TODO: ft
void* SubtitleManager::HideSubtitle(TESObjectREFR* apSpeaker) noexcept
{
#if TP_SKYRIM64
    TP_THIS_FUNCTION(THideSubtitle, void*, SubtitleManager, TESObjectREFR* apSpeaker);
    POINTER_SKYRIMSE(THideSubtitle, s_hideSubtitle, 52627);
    return ThisCall(s_hideSubtitle, this, apSpeaker);
#else
    return nullptr;
#endif
}

void TP_MAKE_THISCALL(HookSubtitleManager, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText,
                      bool aIsInDialogue)
{
    spdlog::debug("Subtitle for actor {:X} (bool {}):\n\t{}", apSpeaker ? apSpeaker->formID : 0, aIsInDialogue, apSubtitleText);

    Actor* pActor = Cast<Actor>(apSpeaker);
    if (pActor && pActor->GetExtension()->IsLocal() && !pActor->GetExtension()->IsPlayer())
        World::Get().GetRunner().Trigger(SubtitleEvent(apSpeaker->formID, apSubtitleText));

    ThisCall(RealShowSubtitle, apThis, apSpeaker, apSubtitleText, aIsInDialogue);
}

TiltedPhoques::Initializer s_subtitleHooks([]()
{
    // TODO: ft
#if TP_SKYRIM64
    POINTER_SKYRIMSE(TShowSubtitle, s_showSubtitle, 52626);

    RealShowSubtitle = s_showSubtitle.Get();

    TP_HOOK(&RealShowSubtitle, HookSubtitleManager);
#endif
});

