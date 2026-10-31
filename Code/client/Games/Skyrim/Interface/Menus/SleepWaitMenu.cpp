#include <Interface/Menus/SleepWaitMenu.h>

#include <Events/TimeSkipRequestEvent.h>
#include <Interface/IMenu.h>
#include <Interface/UI.h>
#include <World.h>

namespace
{
struct GFxNumber
{
    void* pObjectInterface;
    uint32_t type;
    double value;
};

struct FxDelegateArgs
{
    uint8_t responseId[0x18];
    IMenu* pHandler;
    void* pMovie;
    const GFxNumber* pArgs;
    uint32_t argCount;
};
static_assert(offsetof(FxDelegateArgs, pArgs) == 0x28);

const BSFixedString& GetMenuName() noexcept
{
    static const BSFixedString* s_name = new BSFixedString("Sleep/Wait Menu");
    return *s_name;
}

FxDelegateHandler::CallbackFn* RealConfirm = nullptr;

void HookConfirm(const void* apArgs)
{
    if (!World::Get().GetTransport().IsConnected())
        return RealConfirm(apArgs);

    const auto* pArgs = static_cast<const FxDelegateArgs*>(apArgs);
    World::Get().GetRunner().Trigger(TimeSkipRequestEvent{static_cast<float>(pArgs->pArgs[0].value)});
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

    const GFxNumber hours{nullptr, 3, aHours};
    const FxDelegateArgs args{{}, pMenu, nullptr, &hours, 1};
    RealConfirm(&args);
}

static TiltedPhoques::Initializer s_sleepWaitMenuInit(
    []()
    {
        POINTER_SKYRIMSE(FxDelegateHandler::CallbackFn, s_confirm, 524025);

        RealConfirm = s_confirm.Get();

        TP_HOOK(&RealConfirm, HookConfirm);
    });
