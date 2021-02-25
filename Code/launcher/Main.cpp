
#include <Windows.h>
#include "Launcher.h"

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
    ComInitializer comInit;
    TP_UNUSED(comInit);

    auto launcher = TiltedPhoques::MakeUnique<Launcher>(argc, argv);
    if (!launcher->Initialize())
    {
        return -1;
    }

    return launcher->Exec();
}
