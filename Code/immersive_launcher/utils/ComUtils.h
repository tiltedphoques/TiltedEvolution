#pragma once

#include <wrl.h>

// short alias
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct ComScope
{
    ComScope()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        (void)hr;
    }
    ~ComScope()
    {
        CoUninitialize();
    }
};
