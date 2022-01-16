// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <server_plugins/Plugin.h>

namespace node_runtime_plugin
{
class NodeRuntimePlugin final : public server_plugins::Plugin
{
  public:
    NodeRuntimePlugin();
    ~NodeRuntimePlugin();

    void Init() override;
    void Update() override;
    void Detatch() override;

    static server_plugins::Plugin* Create();
    static void Destroy(server_plugins::Plugin* apPlugin);

  private:
};
} // namespace node_runtime_plugin
