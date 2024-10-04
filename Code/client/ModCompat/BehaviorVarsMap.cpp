#ifdef MODDED_BEHAVIOR_COMPATIBILITY
#include "BehaviorVarsMap.h"

const uint32_t BehaviorVarsMap::find(const uint64_t acBehaviorVarsKey, const TiltedPhoques::String acName)
{
    auto map = m_map.find(acBehaviorVarsKey);

    if (map != m_map.end())
    {
        auto map2 = map->second.m_nameMap.find(acName);

        if (map2 != map->second.m_nameMap.end())
            return map2->second;
    }

    return UINT32_MAX;
}

const TiltedPhoques::String BehaviorVarsMap::find(const uint64_t acBehaviorVarsKey, const uint32_t acValue)
{
    const TiltedPhoques::String empty;

    auto map = m_map.find(acBehaviorVarsKey);
    if (map != m_map.end())
    {
        auto map2 = map->second.m_valueMap.find(acValue);

        if (map2 != map->second.m_valueMap.end())
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
