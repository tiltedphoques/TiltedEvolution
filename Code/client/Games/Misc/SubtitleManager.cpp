#include "SubtitleManager.h"
#include "MenuTopicManager.h"

#include <Events/SubtitleEvent.h>

#include <TESObjectREFR.h>
#include <Games/ActorExtension.h>

#include <Forms/TESTopicInfo.h>
#include <Misc/BSFixedString.h>

SubtitleManager* SubtitleManager::Get() noexcept
{
    POINTER_GAME(SubtitleManager*, s_singleton, 400443, 4796374);
    return *s_singleton.Get();
}

#ifdef TP_FALLOUT4
TP_THIS_FUNCTION(TShowSubtitle, void, SubtitleManager, TESObjectREFR* apSpeaker, const BSFixedString& acSubtitleText, TESTopicInfo* apTopicInfo, bool aIsInDialogue);
#else
TP_THIS_FUNCTION(TShowSubtitle, void, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText, bool aIsInDialogue);
#endif
static TShowSubtitle* RealShowSubtitle = nullptr;

void SubtitleManager::ShowSubtitle(TESObjectREFR* apSpeaker, const char* apSubtitleText, TESTopicInfo* apTopicInfo, bool aUnk1) noexcept
{
#ifdef TP_FALLOUT4
    BSFixedString subtitleText(apSubtitleText, true);
    TiltedPhoques::ThisCall(RealShowSubtitle, this, apSpeaker, subtitleText, apTopicInfo, aUnk1);
#else
    TiltedPhoques::ThisCall(RealShowSubtitle, this, apSpeaker, apSubtitleText, aUnk1);
#endif
}

void* SubtitleManager::HideSubtitle(TESObjectREFR* apSpeaker) noexcept
{
#ifdef TP_FALLOUT4
    TP_THIS_FUNCTION(THideSubtitle, void, SubtitleManager, TESObjectREFR* apSpeaker);
    static VersionDbPtr<THideSubtitle> s_hideSubtitle(2249543);
    TiltedPhoques::ThisCall(s_hideSubtitle, this, apSpeaker);
    return nullptr;
#else
    TP_THIS_FUNCTION(THideSubtitle, void*, SubtitleManager, TESObjectREFR* apSpeaker);
    POINTER_SKYRIMSE(THideSubtitle, s_hideSubtitle, 52627);
    return TiltedPhoques::ThisCall(s_hideSubtitle, this, apSpeaker);
#endif
}

#ifdef TP_FALLOUT4
void TP_MAKE_THISCALL(HookShowSubtitle, SubtitleManager, TESObjectREFR* apSpeaker, const BSFixedString& acSubtitleText, TESTopicInfo* apTopicInfo, bool aIsInDialogue)
{
    const char* apSubtitleText = acSubtitleText.AsAscii();
#else
void TP_MAKE_THISCALL(HookShowSubtitle, SubtitleManager, TESObjectREFR* apSpeaker, const char* apSubtitleText, bool aIsInDialogue)
{
#endif
    // spdlog::debug("Subtitle for actor {:X} (bool {}):\n\t{}", apSpeaker ? apSpeaker->formID : 0, aIsInDialogue, apSubtitleText);

    Actor* pActor = Cast<Actor>(apSpeaker);
    const bool isNpc = pActor && !pActor->GetExtension()->IsPlayer();
    const bool shouldSyncSubtitle = apSubtitleText && isNpc && (pActor->GetExtension()->IsLocal() || MenuTopicManager::IsPlayerDialogueSpeaker(pActor));
    if (shouldSyncSubtitle)
        World::Get().GetRunner().Trigger(SubtitleEvent(apSpeaker->formID, apSubtitleText));

#ifdef TP_FALLOUT4
    TiltedPhoques::ThisCall(RealShowSubtitle, apThis, apSpeaker, acSubtitleText, apTopicInfo, aIsInDialogue);
#else
    TiltedPhoques::ThisCall(RealShowSubtitle, apThis, apSpeaker, apSubtitleText, aIsInDialogue);
#endif
}

static TiltedPhoques::Initializer s_subtitleHooks(
    []()
    {
        POINTER_GAME(TShowSubtitle, s_showSubtitle, 52626, 2249542);

        RealShowSubtitle = s_showSubtitle.Get();

        TP_HOOK(&RealShowSubtitle, HookShowSubtitle);
    });
