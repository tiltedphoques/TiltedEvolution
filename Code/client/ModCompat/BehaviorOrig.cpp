#ifdef MODDED_BEHAVIOR_COMPATIBILITY

#include "BehaviorVarsMap.h"
#define MAGIC_ENUM_RANGE_MAX 400
#include "magic_enum.hpp"


namespace BehaviorOrig
{
template <typename V> void GenerateFromEnum(uint64_t aKey)
{
    using namespace magic_enum;
    constexpr size_t count = enum_count<V>();
    constexpr auto variables_values = enum_values<V>();
    constexpr auto variables_names = enum_names<V>();
    BehaviorVars sig;

    sig.m_key = aKey;
    for (size_t index = 0; index < count; index++)
    {
        TiltedPhoques::String name(variables_names[index]);
        uint32_t value = enum_integer(variables_values[index]);

        // Remove the leading 'k'
        name = name.substr(1, name.size() - 1);

        sig.m_nameMap[name]   = value;
        sig.m_valueMap[value] = name;
    }

    BehaviorVarsMap::getInstance().Register(sig);
};

#ifdef TP_SKYRIM
#include "BehaviorOrig.Skyrim"
#else
#include "BehaviorOrig.Fallout4"
#endif

} // using namespace BehaviorOrig

#endif MODDED_BEHAVIOR_COMPATIBILITY
