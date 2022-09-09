#pragma once

#if (!IS_MASTER)
#include "CrashHandler.h"
#elif
#include <crash_handler/CrashHandler.h>
#endif

struct TiltedOnlineApp final : App
{
    TiltedOnlineApp();
    virtual ~TiltedOnlineApp();

    TiltedOnlineApp(TiltedOnlineApp&&) = delete;
    TiltedOnlineApp(const TiltedOnlineApp&) = delete;
    TiltedOnlineApp& operator=(TiltedOnlineApp&&) = delete;
    TiltedOnlineApp& operator=(const TiltedOnlineApp&) = delete;

    [[nodiscard]] void* GetMainAddress() const override;

    bool BeginMain() override;
    bool EndMain() override;

    void Update() override;

    bool Attach() override;
    bool Detach() override;

    static void InstallHooks2();

  protected:
    void InstallHooks();
    void UninstallHooks();

  private:
#if (!IS_MASTER)
    CrashHandler m_crashHandler;
#elif
    ScopedCrashHandler m_crashHandler;
#endif
};
