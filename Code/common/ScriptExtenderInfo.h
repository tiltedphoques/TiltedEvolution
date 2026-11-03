#pragma once

#include <array>
#include <cstdint>

namespace ScriptExtender
{
#if defined(TP_FALLOUT4)
inline constexpr char kName[] = "F4SE";
inline constexpr wchar_t kDllPrefix[] = L"f4se";
inline constexpr wchar_t kPluginDirectory[] = L"Data\\F4SE\\Plugins";
inline constexpr char kEntrypoint[] = "StartF4SE";
// F4SE 0.7.9 for Fallout 4 1.11.240 uses FILEVERSION 0,0,7,9.
inline constexpr std::array<uint32_t, 4> kMinimumFileVersion{0, 0, 7, 9};
#else
inline constexpr char kName[] = "SKSE";
inline constexpr wchar_t kDllPrefix[] = L"skse64";
inline constexpr wchar_t kPluginDirectory[] = L"Data\\SKSE\\Plugins";
inline constexpr char kEntrypoint[] = "StartSKSE";
inline constexpr std::array<uint32_t, 4> kMinimumFileVersion{0, 2, 1, 0};
#endif
} // namespace ScriptExtender
