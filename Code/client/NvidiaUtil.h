#pragma once

#include <Windows.h>
#include <d3d11.h>

bool IsNvidiaOverlayLoaded();

HRESULT CreateEarlyDxDevice(ID3D11Device* apOutDevice, D3D_FEATURE_LEVEL* apOutFeatureLevel);
