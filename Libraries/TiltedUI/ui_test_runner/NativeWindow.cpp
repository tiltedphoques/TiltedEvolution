
#include "NativeWindow.h"

static constexpr wchar_t kWindowTitle[] = L"NG_UI Runner";

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    auto* pSelf = reinterpret_cast<NativeWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!pSelf && msg == WM_NCCREATE)
    {
        pSelf = reinterpret_cast<NativeWindow*>(((LPCREATESTRUCT)lparam)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pSelf);
    }

    if (msg == WM_NCDESTROY)
    {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }

    if (pSelf)
    {
        if (pSelf->DoWndProc(hwnd, msg, wparam, lparam))
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

NativeWindow::NativeWindow(HINSTANCE hs) {
    WNDCLASSEXW winClass = {};
    winClass.cbSize = sizeof(WNDCLASSEXW);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = &WndProc;
    winClass.hInstance = hs;
    winClass.hIcon = LoadIconA(0, IDI_APPLICATION);
    winClass.hCursor = LoadCursorA(0, IDC_ARROW);
    winClass.lpszClassName = L"MyWindowClass";
    winClass.hIconSm = LoadIconA(0, IDI_APPLICATION);

    if (!RegisterClassExW(&winClass)) {
        MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
        return;
    }

    RECT initialRect = { 0, 0, 1024, 768 };
    AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
    LONG initialWidth = initialRect.right - initialRect.left;
    LONG initialHeight = initialRect.bottom - initialRect.top;

    m_hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
        winClass.lpszClassName,
        kWindowTitle,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        initialWidth,
        initialHeight,
        0, 0, hs, (void*)this);

    if (!m_hwnd) {
        MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);
        return;
    }
}

bool NativeWindow::DoWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        if (wparam == VK_ESCAPE)
            DestroyWindow(hwnd);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_SIZE:
    {
        m_bResizing = true;
        break;
    }
    }

    return false;
}
