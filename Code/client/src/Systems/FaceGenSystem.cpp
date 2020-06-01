#include <Systems/FaceGenSystem.h>

#include <Games/References.h>

#include <Games/Skyrim/Forms/BGSHeadPart.h>
#include <Games/Skyrim/Forms/TESNPC.h>

#include <Components.h>
#include <World.h>

#if defined(TP_SKYRIM)

__declspec(noinline) NiTriBasedGeom* GetHeadTriBasedGeom(Actor* apActor, uint32_t aPartType)
{
    using TGetObjectByName = NiAVObject*(BSFaceGenNiNode*, const char**, char);
    POINTER_SKYRIMSE(TGetObjectByName, GetObjectByName, 0x140D41970 - 0x140000000);

    BSFaceGenNiNode* pFaceNode = apActor->GetFaceGenNiNode();
    TESNPC* pActorBase = RTTI_CAST(apActor->baseForm, TESForm, TESNPC);

    if (pFaceNode && pActorBase)
    {
        BGSHeadPart* pFacePart = pActorBase->GetHeadPart(aPartType);

        if (pFacePart)
        {
#if TP_SKYRIM64
            NiAVObject* pHeadNode = GetObjectByName(pFaceNode, &pFacePart->name.data, 1);
#else
            NiAVObject* pHeadNode = pFaceNode->GetByName(pFacePart->name);
#endif

            if (pHeadNode)
            {
                NiTriBasedGeom* pGeometry = pHeadNode->CastToNiTriBasedGeom();

                if (pGeometry)
                {
                    return pGeometry;
                }
            }
        }
    }

    return nullptr;
}

#endif

void FaceGenSystem::Update(World& aWorld, Actor* apActor, FaceGenComponent& aFaceGenComponent) noexcept
{
    //auto pTriBasedGeom = GetHeadTriBasedGeom(apActor, 1);
}

void FaceGenSystem::Setup(World& aWorld, const entt::entity aEntity) noexcept
{
    aWorld.emplace<FaceGenComponent>(aEntity);
}
