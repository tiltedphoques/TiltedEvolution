#include "SubtitleManager.h"

#include <Events/SubtitleEvent.h>

#include <TESObjectREFR.h>
#include <Games/ActorExtension.h>

#include <Forms/TESTopicInfo.h>
#include <Misc/BSFixedString.h>

SubtitleManager* SubtitleManager::Get() noexcept
{
    POINTER_SKYRIMSE(SubtitleManager*, s_singleton, 400443);
    return *s_singleton.Get();
}

TP_THIS_FUNCTION(TShowSubtitle, void, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText, bool aIsInDialogue);
static TShowSubtitle* RealShowSubtitle = nullptr;

void SubtitleManager::ShowSubtitle(TESObjectREFR* apSpeaker, const char* apSubtitleText, TESTopicInfo* apTopicInfo, bool aUnk1) noexcept
{
    TiltedPhoques::ThisCall(RealShowSubtitle, this, apSpeaker, apSubtitleText, aUnk1);
}

// TODO: ft (verify)
void* SubtitleManager::HideSubtitle(TESObjectREFR* apSpeaker) noexcept
{
    TP_THIS_FUNCTION(THideSubtitle, void*, SubtitleManager, TESObjectREFR* apSpeaker);
    POINTER_SKYRIMSE(THideSubtitle, s_hideSubtitle, 52627);
    return TiltedPhoques::ThisCall(s_hideSubtitle, this, apSpeaker);
}

void TP_MAKE_THISCALL(HookShowSubtitle, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText, bool aIsInDialogue)
{
    // spdlog::debug("Subtitle for actor {:X} (bool {}):\n\t{}", apSpeaker ? apSpeaker->formID : 0, aIsInDialogue, apSubtitleText);

    Actor* pActor = Cast<Actor>(apSpeaker);
    if (apSubtitleText && pActor && pActor->GetExtension()->IsLocal() && !pActor->GetExtension()->IsPlayer())
        World::Get().GetRunner().Trigger(SubtitleEvent(apSpeaker->formID, apSubtitleText));

    TiltedPhoques::ThisCall(RealShowSubtitle, apThis, apSpeaker, apSubtitleText, aIsInDialogue);
}

TiltedPhoques::Initializer s_subtitleHooks(
    []()
    {
        POINTER_SKYRIMSE(TShowSubtitle, s_showSubtitle, 52626);

        RealShowSubtitle = s_showSubtitle.Get();

        TP_HOOK(&RealShowSubtitle, HookShowSubtitle);
    });
