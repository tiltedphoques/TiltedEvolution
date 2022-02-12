// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

namespace ImGuiImpl
{
class ImGuiDriver
{
  public:
    ImGuiDriver();
    ~ImGuiDriver();

    void Initialize(void* apNativeHandle);

  private:
    // No user data yet. TBD later
};
} // namespace ImGuiImpl
