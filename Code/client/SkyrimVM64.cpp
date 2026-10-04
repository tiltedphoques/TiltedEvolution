#include <TiltedOnlinePCH.h>
#include "TiltedOnlineApp.h"
#include <Misc/GameVM.h>

extern std::unique_ptr<TiltedOnlineApp> g_appInstance;

#ifdef TP_FALLOUT4
TP_THIS_FUNCTION(TVMUpdate, void, GameVM, float);

static TVMUpdate* VMUpdate = nullptr;

void TP_MAKE_THISCALL(HookVMUpdate, GameVM, float aDelta)
{
    if (!apThis->frozen)
        g_appInstance->Update();

    TiltedPhoques::ThisCall(VMUpdate, apThis, aDelta);
}

static TiltedPhoques::Initializer s_mainHooks(
    []()
    {
        static VersionDbPtr<TVMUpdate> cVMUpdate(2251303);
        VMUpdate = cVMUpdate.Get();
        TP_HOOK(&VMUpdate, HookVMUpdate);
    });
#else
struct Main;

TP_THIS_FUNCTION(TVMUpdate, int, GameVM, float);
TP_THIS_FUNCTION(TMainLoop, short, Main);
TP_THIS_FUNCTION(TVMDestructor, uintptr_t, void);

static TVMUpdate* VMUpdate = nullptr;
static TMainLoop* MainLoop = nullptr;
static TVMDestructor* VMDestructor = nullptr;

int TP_MAKE_THISCALL(HookVMUpdate, GameVM, float a2)
{
    if (apThis->inactive == 0)
        g_appInstance->Update();

    return TiltedPhoques::ThisCall(VMUpdate, apThis, a2);
}

short TP_MAKE_THISCALL(HookMainLoop, Main)
{
    TP_EMPTY_HOOK_PLACEHOLDER

    return TiltedPhoques::ThisCall(MainLoop, apThis);
}

uintptr_t TP_MAKE_THISCALL(HookVMDestructor, void)
{
    TP_EMPTY_HOOK_PLACEHOLDER

    return TiltedPhoques::ThisCall(VMDestructor, apThis);
}

static TiltedPhoques::Initializer s_mainHooks(
    []()
    {
        POINTER_SKYRIMSE(TMainLoop, cMainLoop, 36564);
        POINTER_SKYRIMSE(TVMUpdate, cVMUpdate, 53926);
        POINTER_SKYRIMSE(TVMDestructor, cVMDestructor, 40412);

        VMUpdate = cVMUpdate.Get();
        MainLoop = cMainLoop.Get();
        VMDestructor = cVMDestructor.Get();

        TP_HOOK(&VMUpdate, HookVMUpdate);
        TP_HOOK(&MainLoop, HookMainLoop);
        TP_HOOK(&VMDestructor, HookVMDestructor);
    });

#endif
