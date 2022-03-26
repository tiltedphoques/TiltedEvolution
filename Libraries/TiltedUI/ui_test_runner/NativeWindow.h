#pragma once

#include <Windows.h>

class NativeWindow
{
public:
    explicit NativeWindow(HINSTANCE);

    bool DoWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    bool resizing() {
        return m_bResizing;
    }

    void ResetResized() {
        m_bResizing = false;
    }

    HWND hwnd() {
        return m_hwnd;
    }

private:
    HWND m_hwnd{};
    bool m_bResizing = false;
};
