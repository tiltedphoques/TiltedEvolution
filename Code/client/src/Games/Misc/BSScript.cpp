#include <Games/Skyrim/Misc/BSScript.h>
#include <Games/Skyrim/Misc/NativeFunction.h>

#include <Games/Fallout4/Misc/BSScript.h>
#include <Games/Fallout4/Misc/NativeFunction.h>

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
        POINTER_FALLOUT4(TRegisterPapyrusFunction, s_registerPapyrusFunction, 0x1427338A0 - 0x140000000);

        RealRegisterPapyrusFunction = s_registerPapyrusFunction.Get();

        TP_HOOK(&RealRegisterPapyrusFunction, HookRegisterPapyrusFunction);
    });
