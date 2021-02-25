#pragma once

#include <TiltedCore/Filesystem.hpp>

using namespace TiltedPhoques;

namespace fs = std::filesystem;

// must be kept in sync with frontend!
enum class TitleId : uint32_t
{
    kUnknown,
    kSkyrimSE,
    kSyrimVR,
    kFallout4,
    kFallout4VR,
    kReserved1,
    kReserved2,
};

WString ToClientName(TitleId id) noexcept;
WString ToGameName(TitleId id) noexcept;
TitleId ToTitleId(std::string_view aName) noexcept;
uint32_t ToSteamAppId(TitleId) noexcept;

bool FindTitlePath(TitleId aTitle, bool aForceReselect, fs::path &aPathOut, fs::path &aExeOut);
