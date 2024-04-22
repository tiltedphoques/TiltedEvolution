#ifdef MODDED_BEHAVIOR_COMPATIBILITY
#include "BehaviorVarsMap.h"


uint32_t BehaviorVarsMap::find(const uint64_t acKey, const std::string acName)
{
    auto map = m_map.find(acKey);

    if (map != m_map.end())
    {
        auto map2 = map->second.m_namemap.find(acName);

        if (map2 != map->second.m_namemap.end())
            return map2->second;
    }

    return UINT32_MAX;
}

std::string BehaviorVarsMap::find(const uint64_t acKey, const uint32_t acValue)
{
    constexpr std::string empty;
    
    auto map = m_map.find(acKey);
    if (map != m_map.end())
    {
        auto map2 = map->second.m_valuemap.find(acValue);

        if (map2 != map->second.m_valuemap.end())
            return map2->second;
    }

    return empty;
}

void BehaviorVarsMap::Register(const BehaviorVars aMap)
{
    m_map.insert_or_assign(aMap.m_key, aMap);
}


BehaviorVarsMap& BehaviorVarsMap::getInstance()
{
    // The only instance. Guaranteed lazy initiated
    // Guaranteed that it will be destroyed correctly
    static BehaviorVarsMap instance;

    return instance;
}

#endif MODDED_BEHAVIOR_COMPATIBILITY
