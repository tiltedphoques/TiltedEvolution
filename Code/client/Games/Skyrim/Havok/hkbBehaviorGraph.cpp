#include <Havok/hkbBehaviorGraph.h>

// True if the value lives in the variable's 32-bit value set word (bool, int, real); other types store an index there.
bool hkbBehaviorGraph::IsWordVariable(uint32_t aIndex) const noexcept
{
    // Variable infos are indexed like the variable value set; linking sub-behaviors appends to both.
    if (!data || !data->variableInfos || aIndex >= static_cast<uint32_t>(data->variableInfoCount))
        return false;

    const auto cType = data->variableInfos[aIndex].type;
    return cType >= hkbVariableInfo::kBool && cType <= hkbVariableInfo::kReal;
}
