// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "Action.h"

namespace PluginAPI
{
void ActionStack::PushString(const StringRef acString)
{
    const uint32_t stringLength = static_cast<uint32_t>(std::strlen(acString.data()));

    // store strlen first.
    *reinterpret_cast<size_t*>(&m_pStackBuffer[m_itemOffset]) = stringLength;

    // increment by aligned size..
    m_itemOffset += sizeof(size_t);

    // the whole stack is zero'd by default, so we just move the aligned offset
    memcpy(&m_pStackBuffer[m_itemOffset], acString.data(), stringLength);

    // adjust the offset aligned.
    m_itemOffset += stringLength;
    m_itemOffset += CalculatePadding(m_itemOffset, stringLength);

    // store type info
    m_pTypeInfo[m_itemCount] = ArgType::kString;
    m_itemCount++;
}

void ActionStack::PopString(TiltedPhoques::String& aOutString)
{
    const uint32_t stringLength = *reinterpret_cast<uint32_t*>(&m_pStackBuffer[m_itemOffset]);

    // simulate aligned read.
    m_itemOffset += sizeof(size_t);

    aOutString.assign((const char*)&m_pStackBuffer[m_itemOffset], stringLength);

    // adjust offset aligned.
    m_itemOffset += stringLength;
    m_itemOffset += CalculatePadding(m_itemOffset, stringLength);
}
} // namespace PluginAPI
