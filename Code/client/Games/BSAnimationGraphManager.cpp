#include <TiltedOnlinePCH.h>

#include <BSAnimationGraphManager.h>
#include <Havok/BShkbAnimationGraph.h>
#include <Havok/BShkbHkxDB.h>
#include <Havok/hkbBehaviorGraph.h>

#include <map>

void BSAnimationGraphManager::DumpAnimationVariables(std::map<uint32_t, const char*>& variables, bool aPrintVariables)
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
                for(decltype(pDb->animationVariables.bucketCount) i = 0; i < pDb->animationVariables.bucketCount; ++i)
                {
                    auto pBucket = &pBuckets[i];
                    if (!pBucket->next)
                        continue;

                    while(pBucket != pDb->animationVariables.end)
                    {
                        const auto variableIndex = pBucket->value;
                        if(pVariableSet->size > static_cast<uint32_t>(variableIndex))
                        {
                            variables[variableIndex] = pBucket->key.AsAscii();
                        }

                        pBucket = pBucket->next;
                    }
                }

                if (aPrintVariables)
                {
                    for(auto& [id, name] : variables)
                    {
                        std::cout << "k" << name << " = " << id << "," << std::endl;
                    }
                }
            }
        }
    }
}

std::pair<size_t,size_t> BSAnimationGraphManager::GetDescriptorKey()
{
    std::pair<size_t, size_t> key;
    key.first = key.second = 0;

    std::map<uint32_t, const char*> variables;

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
                for(decltype(pDb->animationVariables.bucketCount) i = 0; i < pDb->animationVariables.bucketCount; ++i)
                {
                    auto pBucket = &pBuckets[i];
                    if (!pBucket->next)
                        continue;

                    while(pBucket != pDb->animationVariables.end)
                    {
                        const auto variableIndex = pBucket->value;
                        if(pVariableSet->size > static_cast<uint32_t>(variableIndex))
                        {
                            variables[variableIndex] = pBucket->key.AsAscii();
                        }

                        pBucket = pBucket->next;
                    }
                }

                for(auto& [id, name] : variables)
                {
                    key.second += strlen(name);
                }
            }
        }
    }

    key.first = variables.size();

    return key;
}
