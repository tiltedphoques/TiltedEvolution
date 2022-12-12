// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "Action.h"

namespace PluginAPI
{
void ActionStack::PushString(const StringRef acString)
{
    const uint32_t stringLength = static_cast<uint32_t>(std::strlen(acString.data()));

    // store strlen first.
    *reinterpret_cast<size_t*>(&m_pArgStack[m_argofs]) = stringLength;

    // increment by aligned size..
    m_argofs += sizeof(size_t);

    // the whole stack is zero'd by default, so we just move the aligned offset
    memcpy(&m_pArgStack[m_argofs], acString.data(), stringLength);

    // adjust the offset aligned.
    m_argofs += stringLength;
    m_argofs += CalculatePadding(m_argofs, stringLength);

    // store type info
    m_pArgTypeStack[m_ArgCount] = ArgType::kString;
    m_ArgCount++;
}

void ActionStack::PopString(TiltedPhoques::String& aOutString)
{
    const uint32_t stringLength = *reinterpret_cast<uint32_t*>(&m_pArgStack[m_argofs]);

    // simulate aligned read.
    m_argofs += sizeof(size_t);

    aOutString.assign((const char*)&m_pArgStack[m_argofs], stringLength);

    // adjust offset aligned.
    m_argofs += stringLength;
    m_argofs += CalculatePadding(m_argofs, stringLength);
}
} // namespace PluginAPI
