
#include <windows.h>
#include <OverlayProc.hpp>
#include "ProcessHandler.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    TiltedPhoques::UIMain(lpCmdLine, hInstance, []() { return new ProcessHandler; });
}
