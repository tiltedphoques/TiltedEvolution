#include <TiltedOnlinePCH.h>
#include "TiltedOnlineApp.h"

#if TP_FALLOUT4

#include <GameVM.h>

extern std::unique_ptr<TiltedOnlineApp> g_appInstance;

struct Main;
struct VMContext
{
    char pad[0x600];
    uint8_t inactive; // 0x600
};

TP_THIS_FUNCTION(TVMUpdate, int, VMContext);
TP_THIS_FUNCTION(TMainLoop, short, Main);
TP_THIS_FUNCTION(TVMDestructor, uintptr_t, void);

static TVMUpdate* VMUpdate = nullptr;
static TMainLoop* MainLoop = nullptr;
static TVMDestructor* VMDestructor = nullptr;

int TP_MAKE_THISCALL(HookVMUpdate, VMContext)
{
    if (apThis->inactive == 0)
        g_appInstance->Update();

    return ThisCall(VMUpdate, apThis);
}

short TP_MAKE_THISCALL(HookMainLoop, Main)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    return ThisCall(MainLoop, apThis);
}

uintptr_t TP_MAKE_THISCALL(HookVMDestructor, void)
{
    TP_EMPTY_HOOK_PLACEHOLDER

    return ThisCall(VMDestructor, apThis);
}

static TiltedPhoques::Initializer s_mainHooks([]()
    {
        POINTER_FALLOUT4(TMainLoop, cMainLoop, 633525);
        POINTER_FALLOUT4(TVMUpdate, cVMUpdate, 759509);
        POINTER_FALLOUT4(TVMDestructor, cVMDestructor, 694341);

        VMUpdate = cVMUpdate.Get();
        MainLoop = cMainLoop.Get();
        VMDestructor = cVMDestructor.Get();

        TP_HOOK(&VMUpdate, HookVMUpdate);
        TP_HOOK(&MainLoop, HookMainLoop);
        TP_HOOK(&VMDestructor, HookVMDestructor);
    });

#endif
