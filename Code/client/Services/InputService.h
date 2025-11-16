#pragma once

#include <Services/KeybindService.h>
#include <internal/cef_types.h>

struct OverlayService;

/**
 * @brief Handles input handling for the UI.
 */
struct InputService
{
    InputService(OverlayService& aOverlay) noexcept;
    ~InputService() noexcept;

    const KeybindService::Key& GetUIKey() const noexcept { return m_pUiKey; }
    bool SetUIKey(const TiltedPhoques::SharedPtr<KeybindService::Key>& acpKey) noexcept;

    static LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Toggle(uint16_t aKey, uint16_t aScanCode, cef_key_event_type_t aType) noexcept;

private:
    // KeybindService should handle any mutations of this key
    KeybindService::Key m_pUiKey;
    bool m_toggleKeyPressed{false};

    TP_NOCOPYMOVE(InputService);
};
