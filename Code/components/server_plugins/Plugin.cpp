// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <server_plugins/Plugin.h>

namespace server_plugins
{

 PluginData::PluginData(const Type actype, void* apModuleHandle, PluginDescriptor* apDesc)
    : m_type(actype), m_pModuleHandle(apModuleHandle), m_pDescriptor(apDesc)
{

}

}
