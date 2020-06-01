#pragma once

struct OverlayService;

struct InputService
{
    InputService(OverlayService& aOverlay) noexcept;
    ~InputService() noexcept;

    TP_NOCOPYMOVE(InputService);
};
