#if TP_SKYRIM

#include <Games/Skyrim/Misc/BSScript.h>
#include <Games/Skyrim/Misc/NativeFunction.h>

TP_THIS_FUNCTION(TRegisterPapyrusFunction, void, BSScript::IVirtualMachine, NativeFunction*);

TRegisterPapyrusFunction* RealRegisterPapyrusFunction = nullptr;

void TP_MAKE_THISCALL(HookRegisterPapyrusFunction, BSScript::IVirtualMachine, NativeFunction* apFunction)
{
    spdlog::info("Register {}::{} with pointer {}", apFunction->typeName.AsAscii(), apFunction->functionName.AsAscii(), apFunction->functionAddress);

    ThisCall(RealRegisterPapyrusFunction, apThis, apFunction);
}

static TiltedPhoques::Initializer s_vmHooks([]()
    {
        POINTER_SKYRIMSE(TRegisterPapyrusFunction, s_registerPapyrusFunction, 0x141258CB0 - 0x140000000);

        RealRegisterPapyrusFunction = s_registerPapyrusFunction.Get();

        TP_HOOK(&RealRegisterPapyrusFunction, HookRegisterPapyrusFunction);
    });

#endif
