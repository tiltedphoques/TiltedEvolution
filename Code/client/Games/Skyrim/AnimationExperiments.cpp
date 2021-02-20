#include <TiltedOnlinePCH.h>

thread_local const char* g_animErrorCode = "";

#include <Games/Animation/TESActionData.h>
#include <Games/References.h>

#include <Misc/BSFixedString.h>
#include <DefaultObjectManager.h>
#include <BSAnimationGraphManager.h>
#include <DefaultStringManager.h>
#include <Games/Animation/ActorMediator.h>

#include <World.h>

#include <Havok/hkbCharacter.h>
#include <Havok/hkbBehaviorGraph.h>
#include <Havok/bhkWorldM.h>
#include <Havok/BShkbAnimationGraph.h>
#include <Havok/CharacterContext.h>
#include <Havok/hkbSymbolIdMap.h>
#include <Havok/hkEventContext.h>
#include <Havok/hkEventType.h>
#include <Havok/hkbGenerator.h>
#include <Havok/BShkbHkxDB.h>

#pragma optimize( "", off )

using TApplyAnimationVariables = void* (void*, TESActionData*);
POINTER_SKYRIMSE(TApplyAnimationVariables, ApplyAnimationVariables, 0x63D0F0);

POINTER_SKYRIMSE(void*, qword_142F271B8, 0x142F271B8 - 0x140000000);

extern thread_local bool g_forceAnimation;

struct Class142F3A1E8
{
    uint8_t SendGraphAction(TESActionData* apAction);
};

void hkbBehaviorGraph::ReSendEvent(hkEventContext& aContext, hkEventType& aType)
{
    TP_THIS_FUNCTION(THandleEvents, void*, hkbBehaviorGraph, hkEventContext&, hkEventType&);
    POINTER_SKYRIMSE(THandleEvents, HandleEvents, 0x9F3CD0);

    if (!aContext.behaviorGraph)
    {
        // The game makes a copy, might as well do the same
        hkEventType type(aType);

        ReHandleEvent(aContext, type);
    }
}

void hkbBehaviorGraph::ReHandleEvent(hkEventContext& aContext, hkEventType& aType)
{
    auto pContext = aContext.characterContext;
    if (!pContext)
        pContext = aContext.character->characterContext;

    const auto cpSymbolId = pContext->symbolIdMap;
    const auto cbyte20 = pContext->byte20;
    const auto cbyte30 = aContext.byte30;

    const auto counter = struct98->count;
    for(auto i = 0; i < counter; ++i)
    {
        const auto pSomeData = &struct98->data[i];
        if(!pSomeData->byte84)
        {
            const auto pGraph = pSomeData->behaviorGraph;
            auto pGenerator = pSomeData->generator;

            pContext->symbolIdMap = pGraph->symbolIdMap;
            pContext->byte20 = 1;

            auto eventType = aType.type;

            if(pGraph->symbolIdMap && eventType >= 0)
            {
                TP_THIS_FUNCTION(Tsub_1409CA9D0, int32_t, void, int32_t, int64_t);
                POINTER_SKYRIMSE(Tsub_1409CA9D0, sub_1409CA9D0, 0x9CA9D0);

                eventType = ThisCall(sub_1409CA9D0, &pGraph->symbolIdMap->pointer20, eventType + 1, -1ll);
            }
            if(eventType != -1)
            {
                aContext.behaviorGraph = pGraph;

                hkEventType type(eventType);
                type.behaviorGraph = aType.behaviorGraph;

                if(pSomeData->byte85 == 0)
                {
                    TP_THIS_FUNCTION(Tsub_140A13150, int32_t, hkEventContext, hkbGenerator*, hkEventType&, SomeData*);
                    POINTER_SKYRIMSE(Tsub_140A13150, sub_140A13150, 0xA13150);

                    ThisCall(sub_140A13150, &aContext, pGenerator, type, pSomeData);
                }
                else
                {
                    pGenerator->SendEvent(aContext, type);
                }
            }
        }
    }

    // dis magic fixes animations
    if(g_forceAnimation)
        aContext.byte30 = 1;

    pContext->symbolIdMap = cpSymbolId;
    pContext->byte20 = cbyte20;

   // auto result = 1;
    if(!(aContext.byte30 & 1))
    {
        TP_THIS_FUNCTION(Tsub_1409F3EF0, void, hkbBehaviorGraph, hkEventContext&, hkEventType&);
        POINTER_SKYRIMSE(Tsub_1409F3EF0, sub_1409F3EF0, 0x9F3EF0);

        hkEventType type(aType);

        ThisCall(sub_1409F3EF0, this, aContext, type);
    }
    if(byte12E || g_forceAnimation)
    {
        TP_THIS_FUNCTION(Tsub_140A4DFA0, void, hkbBehaviorGraph, hkEventContext&);
        POINTER_SKYRIMSE(Tsub_140A4DFA0, sub_140A4DFA0, 0xA4DFA0);

        ThisCall(sub_140A4DFA0, this, aContext);
    }

    aContext.behaviorGraph = nullptr;
    aContext.byte30 = aContext.byte30 || cbyte30;
}

hkEventContext::hkEventContext(hkbCharacter* apCharacter, ahkpWorld* apWorld)
{
    character = apCharacter;
    behaviorGraph = nullptr;
    unk10 = nullptr;
    characterContext = nullptr;
    unk20 = nullptr;
    unk28 = nullptr;
    byte30 = 0;
    hkpWorld = apWorld;
    unk40 = nullptr;
    unk48 = nullptr;
}

hkEventType::hkEventType(int32_t aType)
{
    type = aType;
    behaviorGraph = nullptr;
    pointer10 = nullptr;
}

bool BShkbAnimationGraph::ReSendEvent(BSFixedString* apEventName)
{
    if(behaviorGraph && behaviorGraph->byte12C)
    {
        if(hkxDB->ptr160)
        {
            using Thash_stub = uint32_t(uint32_t&, const char*);
            POINTER_SKYRIMSE(Thash_stub, hash_stub, 0xC06570);

            uint32_t hash = 0;
            hash_stub(hash, apEventName->data);

            const auto pBuckets = hkxDB->hashTable.buckets;
            if(pBuckets)
            {
                const auto mask = hkxDB->hashTable.bucketCount - 1;

                auto pEntry = &pBuckets[hash & mask];
                if(pEntry->next)
                {
                    while(pEntry->key != apEventName->data)
                    {
                        pEntry = pEntry->next;
                        if (pEntry == hkxDB->hashTable.end)
                            return false;
                    }

                    ahkpWorld* pWorld = nullptr;
                    if (hkWorldM)
                        pWorld = hkWorldM->GetWorld();

                    auto* pCharacter = &character;

                    hkEventContext context(pCharacter, pWorld);
                    hkEventType type(pEntry->value);

                    // 1409f0360
                    behaviorGraph->ReSendEvent(context, type);

                    return context.byte30 != 0;
                }
            }
        }
    }

    return false;

    /*
    TP_THIS_FUNCTION(TBSSendEvent, bool, BShkbAnimationGraph, BSFixedString*);
    POINTER_SKYRIMSE(TBSSendEvent, InternalSendEvent, 0xAEFEB0);

    return ThisCall(InternalSendEvent, this, apEventName);*/
}


uint32_t BSAnimationGraphManager::ReSendEvent(BSFixedString* apEventName)
{
    BSScopedLock<BSRecursiveLock> _(lock);

    uint32_t counter = 0;

    for(auto i = 0; i < animationGraphs.size; ++i)
    {
        auto pCurrentAnimationGraph = animationGraphs.Get(i);
        if (pCurrentAnimationGraph->ReSendEvent(apEventName))
            ++counter;
    }

    // Not sure what this is, looks like it call this function on another graph manager, maybe to sync ridder/ridden ?
    TP_THIS_FUNCTION(Tsub_140AE2DB0, uint8_t, BSAnimationGraphManager, BSFixedString*);
    POINTER_SKYRIMSE(Tsub_140AE2DB0, sub_140AE2DB0, 0xAE2DB0);

    ThisCall(sub_140AE2DB0, this, apEventName);

    return counter;
}

bool IAnimationGraphManagerHolder::ReSendAnimationEvent(BSFixedString* apAnimEvent)
{
    BSAnimationGraphManager* pAnimationGraph = nullptr;
    auto result = GetBSAnimationGraph(&pAnimationGraph);

    TP_THIS_FUNCTION(TBSSendEvent, uint32_t, BSAnimationGraphManager, BSFixedString*);
    POINTER_SKYRIMSE(TBSSendEvent, BSSendEvent, 0xAE24A0);

    if(pAnimationGraph)
    {
        result = pAnimationGraph->ReSendEvent(apAnimEvent) != 0;

        pAnimationGraph->Release();
    }

    return result;
}

uint8_t Class142F3A1E8::SendGraphAction(TESActionData* apAction)
{
    TP_THIS_FUNCTION(TGetSomeIdleProperty, uint8_t, TESIdleForm);
    POINTER_SKYRIMSE(TGetSomeIdleProperty, GetSomeIdleProperty, 0x3580C0);

    if (!apAction->actor)
        return 0;

    if(apAction->idleForm)
    {
        auto& objectManager = DefaultObjectManager::Get();
        BGSAction* pAction = nullptr;
        if(objectManager.isSomeActionReady)
        {
            pAction = objectManager.someAction;
        }

        if(pAction == apAction->action)
        {
            const auto result = static_cast<float>(ThisCall(GetSomeIdleProperty, apAction->idleForm));

            auto& stringManager = DefaultStringManager::Get();

            apAction->actor->animationGraphHolder.SetVariableFloat(&stringManager.fIdleTimer, result);
        }
    }

    return apAction->actor->animationGraphHolder.ReSendAnimationEvent(&apAction->eventName);
}

bool ActorMediator::RePerformAction(TESActionData* apAction) noexcept
{
    TP_THIS_FUNCTION(TAnimationStep, uint8_t, ActorMediator, TESActionData*);

    POINTER_SKYRIMSE(TAnimationStep, PerformIdleAction, 0x63B060);
    POINTER_SKYRIMSE(TAnimationStep, PerformComplexAction, 0x63B0F0);

    uint8_t result = 0;

    auto pActor = static_cast<Actor*>(apAction->actor);
    if(!pActor || pActor->animationGraphHolder.IsReady())
    {
        //result = ThisCall(PerformIdleAction, this, apAction);

        //if (apAction->someFlag & BGSActionData::kTransitionNoAnimation)
        //    apAction->someFlag = BGSActionData::kSkip;
        //else
        {
            result = ThisCall(PerformComplexAction, this, apAction);
            //result = RePerformComplexAction(apAction);
        }

        ApplyAnimationVariables(*qword_142F271B8.Get(), apAction);

        /*if (result == 0)
            g_animErrorCode = "PerformIdleAction failed";

        if(result)
        {
            if (apAction->someFlag & BGSActionData::kTransitionNoAnimation)
                apAction->someFlag = BGSActionData::kSkip;
            else
            {
                result = ThisCall(PerformComplexAction, this, apAction);
                //result = RePerformComplexAction(apAction);
            }
        }
        else if ((apAction->someFlag & BGSActionData::kTransitionNoAnimation) == 0)
        {
            ApplyAnimationVariables(*qword_142F271B8.Get(), apAction);
        }*/
    }

    return result;
}

bool ActorMediator::RePerformIdleAction(TESActionData* apData) noexcept
{
    using Tsub_14063CAA0 = bool(void*, TESActionData*);
    using Tsub_14063CFB0 = bool(void*, TESActionData*);
    using Tsub_1405CCB20 = bool(void*, TESActionData*);
    using Tsub_1404ECF50 = bool(void*, TESActionData*);

    POINTER_SKYRIMSE(Tsub_14063CAA0, sub_14063CAA0, 0x14063CAA0 - 0x140000000);
    POINTER_SKYRIMSE(Tsub_14063CFB0, sub_14063CFB0, 0x14063CFB0 - 0x140000000);
    POINTER_SKYRIMSE(Tsub_1405CCB20, sub_1405CCB20, 0x1405CCB20 - 0x140000000);
    POINTER_SKYRIMSE(Tsub_1404ECF50, sub_1404ECF50, 0x1404ECF50 - 0x140000000);

    POINTER_SKYRIMSE(void*, qword_142F271C8, 0x142F271C8 - 0x140000000);
    POINTER_SKYRIMSE(void*, qword_142EFF990, 0x142EFF990 - 0x140000000);

    bool result = true;

    if(((apData->someFlag >> 1) & 1) == 0)
    {
        if(!sub_14063CAA0(*qword_142F271B8.Get(), apData))
        {
            result = false;
        }

        if (sub_1405CCB20(*qword_142F271C8.Get(), apData))
        {
            if (result && !sub_14063CFB0(*qword_142F271B8.Get(), apData))
                result = false;
        }
        else
        {
            result = false;
        }

        // Has something to do with the target refr
        sub_1404ECF50(*qword_142EFF990.Get(), apData);
    }

    return result;
}

bool ActorMediator::RePerformComplexAction(TESActionData* apData) noexcept
{
    TP_THIS_FUNCTION(Tsub_1404ED090, uint8_t, void, TESActionData*);
    
    using Tsub_1401A2220 = void(BSFixedString*);

    POINTER_SKYRIMSE(Tsub_1404ED090, sub_1404ED090, 0x4ED090);
    POINTER_SKYRIMSE(Tsub_1401A2220, sub_1401A2220, 0x1A2220);

    POINTER_SKYRIMSE(void, qword_142EFF990, 0x2EFF990);
    POINTER_SKYRIMSE(Class142F3A1E8, qword_142F3A1E8, 0x2F3A1E8);

    auto v4 = false;
    if (apData->ComputeResult())
        v4 = apData->result == 2;

    auto result = ThisCall(sub_1404ED090, qword_142EFF990, apData);
    if (result)
    {
        qword_142F3A1E8->SendGraphAction(apData);
    }

    ApplyAnimationVariables(*qword_142F271B8.Get(), apData);

    if (!v4)
        return result;

    apData->someFlag &= 0xFFFFFFFC;
    sub_1401A2220(&apData->eventName);

    result = apData->Perform() & 0xFF;

    return result;
}


bool TESActionData::ComputeResult()
{
    using Tsub_1401A1D50 = char(void*, TESActionData*);
    POINTER_SKYRIMSE(Tsub_1401A1D50, sub_1401A1D50, 0x1A1D50);

    result = 0;
    if (targetIdleForm)
    {
        if (!_strnicmp(eventName, "NA_", 3))
        {
            result = 2;
            return true;
        }

        auto v3 = sub_3();
        if (v3)
        {
            result = 1;
            sub_1401A1D50(v3, this);
        }
    }

    return result > 0;
}

#pragma optimize( "", on )
