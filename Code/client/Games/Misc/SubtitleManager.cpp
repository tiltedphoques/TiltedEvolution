#include "SubtitleManager.h"

#include <Events/SubtitleEvent.h>

#include <TESObjectREFR.h>
#include <Games/ActorExtension.h>

#include <Forms/TESTopicInfo.h>
#include <Misc/BSFixedString.h>

SubtitleManager* SubtitleManager::Get() noexcept
{
    POINTER_SKYRIMSE(SubtitleManager*, s_singleton, 400443);
    POINTER_FALLOUT4(SubtitleManager*, s_singleton, 740512);
    return *s_singleton.Get();
}

#if TP_SKYRIM64
TP_THIS_FUNCTION(TShowSubtitle, void, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText,
                 bool aIsInDialogue);
#elif TP_FALLOUT4
TP_THIS_FUNCTION(TShowSubtitle, void, SubtitleManager, TESObjectREFR* apSpeaker, BSFixedString* apSubtitleText,
                 TESTopicInfo* apTopicInfo, bool aIsInDialogue);
#endif
static TShowSubtitle* RealShowSubtitle = nullptr;

void SubtitleManager::ShowSubtitle(TESObjectREFR* apSpeaker, const char* apSubtitleText, TESTopicInfo* apTopicInfo, bool aUnk1) noexcept
{
#if TP_SKYRIM64
    ThisCall(RealShowSubtitle, this, apSpeaker, apSubtitleText, aUnk1);
#elif TP_FALLOUT4
    BSFixedString subtitleText(apSubtitleText);
    ThisCall(RealShowSubtitle, this, apSpeaker, &subtitleText, apTopicInfo, aUnk1);
    subtitleText.Release();
#endif
}

// TODO: ft (verify)
void* SubtitleManager::HideSubtitle(TESObjectREFR* apSpeaker) noexcept
{
    TP_THIS_FUNCTION(THideSubtitle, void*, SubtitleManager, TESObjectREFR* apSpeaker);
    POINTER_SKYRIMSE(THideSubtitle, s_hideSubtitle, 52627);
    POINTER_FALLOUT4(THideSubtitle, s_hideSubtitle, 755148);
    return ThisCall(s_hideSubtitle, this, apSpeaker);
}

#if TP_SKYRIM64
void TP_MAKE_THISCALL(HookShowSubtitle, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText,
                      bool aIsInDialogue)
#elif TP_FALLOUT4
void TP_MAKE_THISCALL(HookShowSubtitle, SubtitleManager, TESObjectREFR* apSpeaker, BSFixedString* apSubtitleText,
                      TESTopicInfo* apTopicInfo, bool aIsInDialogue)
#endif
{
    //spdlog::debug("Subtitle for actor {:X} (bool {}):\n\t{}", apSpeaker ? apSpeaker->formID : 0, aIsInDialogue, apSubtitleText);

    Actor* pActor = Cast<Actor>(apSpeaker);
    if (pActor && pActor->GetExtension()->IsLocal() && !pActor->GetExtension()->IsPlayer())
#if TP_SKYRIM64
        World::Get().GetRunner().Trigger(SubtitleEvent(apSpeaker->formID, apSubtitleText));
#elif TP_FALLOUT4
        World::Get().GetRunner().Trigger(SubtitleEvent(apSpeaker->formID, apSubtitleText->AsAscii()));
#endif


#if TP_SKYRIM64
    ThisCall(RealShowSubtitle, apThis, apSpeaker, apSubtitleText, aIsInDialogue);
#elif TP_FALLOUT4
    ThisCall(RealShowSubtitle, apThis, apSpeaker, apSubtitleText, apTopicInfo, aIsInDialogue);
#endif
}

TiltedPhoques::Initializer s_subtitleHooks([]()
{
    POINTER_SKYRIMSE(TShowSubtitle, s_showSubtitle, 52626);
    POINTER_FALLOUT4(TShowSubtitle, s_showSubtitle, 875509);

    RealShowSubtitle = s_showSubtitle.Get();

    TP_HOOK(&RealShowSubtitle, HookShowSubtitle);
});

