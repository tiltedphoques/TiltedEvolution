#pragma once

#include <spdlog/spdlog.h>

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Platform.hpp>
#include "TitleSelect.h"

class Launcher
{
 public:
    enum class AppState
    {
        kFailed,
        kStarting,
        kInGame,
        kBackground
    };

    explicit Launcher(int argc, char** argv);
    ~Launcher();

    bool Initialize();
    void LoadClient();
    void StartGame(TitleId aTid);

    int32_t Exec() noexcept;

    const fs::path& GetGamePath() const;
    const fs::path& GetExePath() const;

  private:
    AppState m_appState{AppState::kStarting};
    TitleId m_titleId{TitleId::kUnknown};

    bool m_bReselectFlag = false;
    HMODULE m_pGameClientHandle = nullptr;
    fs::path m_gamePath;
    fs::path m_exePath;
};
