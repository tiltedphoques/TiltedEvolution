#include <Games/Skyrim/BSAnimationGraphManager.h>
#include <Games/Skyrim/Havok/BShkbAnimationGraph.h>
#include <Games/Skyrim/Havok/BShkbHkxDB.h>
#include <Games/Skyrim/Havok/hkbBehaviorGraph.h>

#include <Games/Fallout4/BSAnimationGraphManager.h>
#include <Games/Fallout4/Havok/BShkbAnimationGraph.h>
#include <Games/Fallout4/Havok/BShkbHkxDB.h>
#include <Games/Fallout4/Havok/hkbBehaviorGraph.h>

#include <map>

void BSAnimationGraphManager::DumpAnimationVariables()
{
    if (animationGraphIndex < animationGraphs.size)
    {
        const auto pGraph = animationGraphs.Get(animationGraphIndex);
        if(pGraph)
        {
            const auto pDb = pGraph->hkxDB;
            const auto pBuckets = pDb->animationVariables.buckets;
            const auto pVariableSet = pGraph->behaviorGraph->animationVariables;

            if(pBuckets && pVariableSet)
            {
                std::map<uint32_t, const char*> variables;

                for(auto i = 0; i < pDb->animationVariables.bucketCount; ++i)
                {
                    auto pBucket = &pBuckets[i];
                    if (!pBucket->next)
                        continue;

                    while(pBucket != pDb->animationVariables.end)
                    {
                        const auto variableIndex = pBucket->value;
                        if(pVariableSet->size > variableIndex)
                        {
                            const auto value = pVariableSet->data[variableIndex];

                            variables[variableIndex] = pBucket->key.AsAscii();
                        }

                        pBucket = pBucket->next;
                    }
                }

                for(auto& var : variables)
                {
                    spdlog::info("{} {}", var.first, var.second);
                }
            }
        }
    }
}
