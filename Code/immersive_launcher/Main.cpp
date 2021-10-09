
#include <Windows.h>
#include "launcher.h"
#include "Utils/Error.h"

#include "script_extender/SEMemoryBlock.h"

extern void CoreStubsInit();

extern "C"
{
    __declspec(dllexport) int NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

struct ComScope
{
    ComScope()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        TP_UNUSED(hr);
    }
    ~ComScope()
    {
        CoUninitialize();
    }
};

int main(int argc, char** argv)
{
    // memory block for Script Extender reserved as early as we can
    script_extender::SEMemoryBlock b;
    if (!b.Good())
    {
        Die("Failed to pre-reserve script extender zone.\nAsk Force!");
        return -1;
    }
    CoreStubsInit();

    ComScope cs;
    TP_UNUSED(cs);

    return launcher::StartUp(argc, argv);
}
