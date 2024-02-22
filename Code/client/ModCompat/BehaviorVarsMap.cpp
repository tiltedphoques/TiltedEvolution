#include "BehaviorVarsMap.h"


uint32_t BehaviorVarsMap::find(const uint64_t key, const std::string name)
{
    auto map = m_map.find(key);

    if (map != m_map.end())
    {
        auto map2 = map->second.m_namemap.find(name);

        if (map2 != map->second.m_namemap.end())
            return map2->second;
    }

    return -1;
}

std::string BehaviorVarsMap::find(const uint64_t key, const uint32_t value)
{
    constexpr std::string empty;
    
    auto map = m_map.find(key);
    if (map != m_map.end())
    {
        auto map2 = map->second.m_valuemap.find(value);

        if (map2 != map->second.m_valuemap.end())
            return map2->second;
    }

    return empty;
}

void BehaviorVarsMap::Register(const BehaviorVars map)
{
    m_map.insert_or_assign(map.m_key, map);
}


BehaviorVarsMap& BehaviorVarsMap::getInstance()
{
    // The only instance. Guaranteed lazy initiated
    // Guaranteed that it will be destroyed correctly
    static BehaviorVarsMap instance;

    return instance;
}

