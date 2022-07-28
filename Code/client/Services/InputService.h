#pragma once

struct OverlayService;

/**
* @brief Handles input handling for the UI.
*/
struct InputService
{
    InputService(OverlayService& aOverlay) noexcept;
    ~InputService() noexcept;

    static LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    TP_NOCOPYMOVE(InputService);
};
