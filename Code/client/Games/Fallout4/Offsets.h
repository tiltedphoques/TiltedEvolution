#pragma once
// Fallout4 1.11.240 named offsets.
//
// The F4SE all-in-one address library (version-1-11-240-0.bin) carries ids
// but no names, and its id space is missing the RTTI block. This module is
// the name-keyed complement: a table of symbol name -> RVA generated from
//   * an offline reference symbol table (1.10.155) bridged into 1.11.240
//     by unique prologue match,
//   * the libxse/commonlibf4 next-gen id tables resolved through the
//     address library,
//   * RTTI type descriptors read straight out of the 1.11.240 image.
//
// Everything is keyed by RVA against the exact 1.11.240 build, guarded by
// the version check in RunTiltedInit.
#pragma once

#include <cstdint>
#include <string_view>

namespace fo4
{
// Resolves a symbol name to its RVA, or 0 when unknown.
uint32_t Rva(std::string_view acName) noexcept;

// Resolves and adds the module base. Returns nullptr when unknown.
void* Address(std::string_view acName) noexcept;

// True when the named RTTI type descriptor exists for this build.
bool HasRtti(std::string_view acTypeName) noexcept;

// RTTI type descriptor address (already rebased), nullptr when unknown.
const void* Rtti(std::string_view acTypeName) noexcept;
} // namespace fo4
