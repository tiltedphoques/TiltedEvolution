#include <TiltedOnlinePCH.h>

#if TP_SKYRIM64

#include <GameVM.h>

class Main;
struct VMContext
{
    char pad[0x680];
    uint8_t inactive; // 0x680
};

TP_THIS_FUNCTION(TVMUpdate, int, VMContext, float);
TP_THIS_FUNCTION(TMainLoop, short, Main);
TP_THIS_FUNCTION(TVMDestructor, uintptr_t, void);

static TVMUpdate* VMUpdate = nullptr;
static TMainLoop* MainLoop = nullptr;
static TVMDestructor* VMDestructor = nullptr;

int TP_MAKE_THISCALL(HookVMUpdate, VMContext, float a2)
{
    if (apThis->inactive == 0)
        TiltedPhoques::App::GetInstance().Update();

    return ThisCall(VMUpdate, apThis, a2);
}

short TP_MAKE_THISCALL(HookMainLoop, Main)
{
    TP_EMPTY_HOOK_PLACEHOLDER

    return ThisCall(MainLoop, apThis);
}

uintptr_t TP_MAKE_THISCALL(HookVMDestructor, void)
{
    TP_EMPTY_HOOK_PLACEHOLDER

    return ThisCall(VMDestructor, apThis);
}

static TiltedPhoques::Initializer s_vmHooks([]()
    {
        POINTER_SKYRIMSE(TMainLoop, cMainLoop, 0x1405B2FF0 - 0x140000000);
        POINTER_SKYRIMSE(TVMUpdate, cVMUpdate, 0x140921F10 - 0x140000000);
        POINTER_SKYRIMSE(TVMDestructor, cVMDestructor, 0x140699DF0 - 0x140000000);

        VMUpdate = cVMUpdate.Get();
        MainLoop = cMainLoop.Get();
        VMDestructor = cVMDestructor.Get();

        TP_HOOK(&VMUpdate, HookVMUpdate);
        TP_HOOK(&MainLoop, HookMainLoop);
        TP_HOOK(&VMDestructor, HookVMDestructor);
    });

#endif
