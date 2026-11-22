#include <Interface/Menus/SleepWaitMenu.h>

#include <Events/TimeSkipRequestEvent.h>
#include <Interface/UI.h>
#include <World.h>

#include <atomic>

namespace
{
const BSFixedString& GetMenuName() noexcept
{
    static const BSFixedString* s_name = new BSFixedString("Sleep/Wait Menu");
    return *s_name;
}

FxDelegateHandler::CallbackFn* RealConfirm = nullptr;

TP_THIS_FUNCTION(TProcessMessage, UI_MESSAGE_RESULTS, IMenu, UIMessage&);
TProcessMessage* RealProcessMessage = nullptr;

// Hours of a shared skip that waits for the menu's next update, 0 if none.
std::atomic<float> s_pendingHours{0.f};

void HookConfirm(const FxDelegateArgs* apArgs)
{
    if (!World::Get().GetTransport().IsConnected())
        return RealConfirm(apArgs);

    World::Get().GetRunner().Trigger(TimeSkipRequestEvent{static_cast<float>((*apArgs)[0].value.number)});
}

// The menu's own updates run on the main thread, and they also drive the sleep once it started.
UI_MESSAGE_RESULTS TP_MAKE_THISCALL(HookProcessMessage, IMenu, UIMessage& aMessage)
{
    const UI_MESSAGE_RESULTS result = TiltedPhoques::ThisCall(RealProcessMessage, apThis, aMessage);

    if (aMessage.eType == UIMessage::kHide || aMessage.eType == UIMessage::kForceHide)
        s_pendingHours = 0.f;

    if (aMessage.eType != UIMessage::kUpdate)
        return result;

    const float pendingHours = s_pendingHours.exchange(0.f);
    if (pendingHours <= 0.f)
        return result;

    GFxValue hours{};
    hours.type = GFxValue::kNumber;
    hours.value.number = pendingHours;

    FxDelegateArgs args{};
    args.pHandler = apThis;
    args.pArgs = &hours;
    args.numArgs = 1;
    RealConfirm(&args);

    return result;
}
} // namespace

bool SleepWaitMenu::IsOpen() noexcept
{
    return UI::Get()->GetMenuOpen(GetMenuName());
}

void SleepWaitMenu::Confirm(float aHours) noexcept
{
    // Network messages arrive on a job thread the main thread waits for, and starting to sleep there
    // deadlocks once the sleep waits for the main thread. Leave it to the menu's next update.
    if (IsOpen())
        s_pendingHours = aHours;
}

static TiltedPhoques::Initializer s_sleepWaitMenuInit(
    []()
    {
        // SleepWaitMenu's "OK" callback, the one SleepWaitMenu::Accept registers first. 1.7.104 gave it a new ID.
#ifdef TP_SKYRIM_1_6_1170
        POINTER_SKYRIMSE(FxDelegateHandler::CallbackFn, s_confirm, 52488);
#else
        POINTER_SKYRIMSE(FxDelegateHandler::CallbackFn, s_confirm, 524025);
#endif

        POINTER_SKYRIMSE(TProcessMessage, s_processMessage, 52486);

        RealConfirm = s_confirm.Get();
        RealProcessMessage = s_processMessage.Get();

        TP_HOOK(&RealConfirm, HookConfirm);
        TP_HOOK(&RealProcessMessage, HookProcessMessage);
    });
