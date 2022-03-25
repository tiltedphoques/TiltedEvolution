#pragma once

#include "OverlayClient.h"

namespace TiltedPhoques
{
    struct RenderProvider;

    class OverlaySpace
    {
    public:
        [[nodiscard]] OverlayClient* GetClient() const noexcept { return m_pClient.get(); };

        void LoadContent(RenderProvider* apRenderer);

        // Dev tools are a per space thing, like each tab in chrome can have its own dev tools
        bool ShowDevTools();


        void ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments = nullptr) const noexcept;

        // movement
        void InjectKey(cef_key_event_type_t aType, uint32_t aModifiers, uint16_t aKey, uint16_t aScanCode) const noexcept;
        void InjectMouseButton(uint16_t aX, uint16_t aY, cef_mouse_button_type_t aButton, bool aUp, uint32_t aModifier) const noexcept;
        void InjectMouseMove(uint16_t aX, uint16_t aY, uint32_t aModifier) const noexcept;
        void InjectMouseWheel(uint16_t aX, uint16_t aY, int16_t aDelta, uint32_t aModifier) const noexcept;
    private:
        CefRefPtr<OverlayClient> m_pClient;
    };
}
