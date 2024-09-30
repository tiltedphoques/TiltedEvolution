#pragma once

#include <Services/KeybindService.h>

struct OverlayService;

/**
 * @brief Handles input handling for the UI.
 */
struct InputService
{
    InputService(OverlayService& aOverlay) noexcept;
    ~InputService() noexcept;

    // Use this function to get the player's current UI key
    const KeybindService::Key& GetUIKey() const noexcept { return m_pUiKey; }
    bool SetUIKey(std::shared_ptr<KeybindService::Key> apKey) noexcept;

    static LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // KeybindService should handle any mutations of this key
    KeybindService::Key m_pUiKey;

    TP_NOCOPYMOVE(InputService);
};
