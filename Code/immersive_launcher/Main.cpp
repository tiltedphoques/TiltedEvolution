
#include "Utils/Error.h"
#include "launcher.h"
#include <Windows.h>
#include <combaseapi.h>
#include <base/threading/ThreadUtils.h>

#include "utils/ComUtils.h"
#include "script_extender/SEMemoryBlock.h"
#include <crash_handler/CrashHandler.h>

#include <TiltedCore/Platform.hpp>

extern void CoreStubsInit();

extern "C"
{
    __declspec(dllexport) int NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

auto kSystemPreloadDlls = {L"\\dinput8.dll", // < Skyrim early init hook
                           L"\\dsound.dll",  // < breaks DSound init in game code
                                             // < X360CE v3 is buggy with COM hooks
                           L"\\xinput9_1_0.dll", L"\\xinput1_1.dll", L"\\xinput1_2.dll", L"\\xinput1_3.dll",
                           L"\\xinput1_4.dll", L"\\version.dll"};

static void PreloadSystemDlls()
{
    auto loadSystemDll = [](auto dll) {
        wchar_t systemPath[512];
        GetSystemDirectoryW(systemPath, _countof(systemPath));

        wcscat_s(systemPath, dll);

        LoadLibraryW(systemPath);
    };

    for (auto dll : kSystemPreloadDlls)
        loadSystemDll(dll);
}

int main(int argc, char** argv)
{
    Base::SetCurrentThreadName("MainLauncherThread");

    // memory block for Script Extender reserved as early as we can
    script_extender::SEMemoryBlock b;
    if (!b.Good())
    {
        Die(L"Failed to pre-reserve script extender zone.\nAsk Force!");
        return -1;
    }

    PreloadSystemDlls();
    CoreStubsInit();

    ComScope cs;
    TP_UNUSED(cs);

    ScopedCrashHandler _;
    auto ret = launcher::StartUp(argc, argv);

    return ret;
}
