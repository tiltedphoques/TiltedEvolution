#pragma once

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

protected:

    void InstallHooks();
    void UninstallHooks();

};
