// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

namespace ImGuiImpl
{
class ImGuiDriver
{
  public:
    ImGuiDriver();
    ~ImGuiDriver();

  private:
    // No user data yet. TBD later
};
} // namespace ImGuiImpl
