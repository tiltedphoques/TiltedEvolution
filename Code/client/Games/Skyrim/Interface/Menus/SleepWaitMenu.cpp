#include <Interface/Menus/SleepWaitMenu.h>

#include <Events/TimeSkipRequestEvent.h>
#include <Interface/UI.h>
#include <World.h>

namespace
{
const BSFixedString& GetMenuName() noexcept
{
    static const BSFixedString* s_name = new BSFixedString("Sleep/Wait Menu");
    return *s_name;
}

FxDelegateHandler::CallbackFn* RealConfirm = nullptr;

void HookConfirm(const FxDelegateArgs* apArgs)
{
    if (!World::Get().GetTransport().IsConnected())
        return RealConfirm(apArgs);

    World::Get().GetRunner().Trigger(TimeSkipRequestEvent{static_cast<float>((*apArgs)[0].value.number)});
}
} // namespace

bool SleepWaitMenu::IsOpen() noexcept
{
    return UI::Get()->GetMenuOpen(GetMenuName());
}

void SleepWaitMenu::Confirm(float aHours) noexcept
{
    IMenu* pMenu = UI::Get()->FindMenuByName(GetMenuName());
    if (!pMenu)
        return;

    GFxValue hours{};
    hours.type = GFxValue::kNumber;
    hours.value.number = aHours;

    FxDelegateArgs args{};
    args.pHandler = pMenu;
    args.pArgs = &hours;
    args.numArgs = 1;
    RealConfirm(&args);
}

static TiltedPhoques::Initializer s_sleepWaitMenuInit(
    []()
    {
        POINTER_SKYRIMSE(FxDelegateHandler::CallbackFn, s_confirm, 524025);

        RealConfirm = s_confirm.Get();

        TP_HOOK(&RealConfirm, HookConfirm);
    });
