
#include "RemoteControl.h"
#include "Pch.h"

// This is a great resource that i used while embedding python:
// https://pybind11.readthedocs.io/en/stable/advanced/embedding.html
RemoteControlPlugin::~RemoteControlPlugin()
{
}

bool RemoteControlPlugin::Initialize()
{

    PluginAPI::PluginAPI_WriteLog(PluginAPI::LogLevel::kInfo, "Initialized remote control plugin");
    return true;
}

void RemoteControlPlugin::Shutdown()
{
    PluginAPI::PluginAPI_WriteLog(PluginAPI::LogLevel::kInfo, "Shutting down remote control plugin");
}

uint32_t RemoteControlPlugin::OnEvent(uint32_t aEventCode)
{
    return EventResult::kEventResultContinue;
}

void RemoteControlPlugin::OnTick()
{
}
