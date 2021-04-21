
#include <Windows.h>
#include "Launcher.h"
#include "Utils/Error.h"
#include "Utils/RipZone.h"

extern void CoreStubsInit();

extern "C"
{
    __declspec(dllexport) int NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

struct ComInitializer
{
    ComInitializer()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        TP_UNUSED(hr);
    }
    ~ComInitializer()
    {
        CoUninitialize();
    }
};

int main(int argc, char** argv)
{
    CoreStubsInit();
    if (!LowRipZoneInit())
    {
        FatalError(L"Failed to initialize rip zone.\nCannot continue!");
        return -1;
    }

    ComInitializer comInit;
    TP_UNUSED(comInit);

    auto launcher = TiltedPhoques::MakeUnique<Launcher>(argc, argv);

    if (!launcher->Initialize())
    {
        return -2;
    }

    return launcher->Exec();
}
