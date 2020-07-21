#include <Systems/FaceGenSystem.h>

#include <Games/References.h>

#include <Games/Skyrim/Forms/BGSHeadPart.h>
#include <Games/Skyrim/Forms/TESNPC.h>

#include <Components.h>
#include <World.h>

#include <Structs/Tints.h>

#if TP_SKYRIM

#include <Games/Skyrim/NetImmerse/NiTriBasedGeom.h>
#include <Games/Skyrim/NetImmerse/NiRenderedTexture.h>
#include <Games/Skyrim/NetImmerse/BSShaderProperty.h>
#include <Games/Skyrim/NetImmerse/BSLightingShaderProperty.h>
#include <Games/Skyrim/NetImmerse/BSMaskedShaderMaterial.h>
#include <Games/Memory.h>

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
            NiAVObject* pHeadNode = GetObjectByName(pFaceNode, &pFacePart->name.data, 1);

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

struct TextureHolder;
TP_THIS_FUNCTION(TCreateResourceView, Ni2DBuffer*, TextureHolder, uint32_t, uint32_t);

using TCreateTexture = NiRenderedTexture * (__fastcall)(BSFixedString& aName);
using TCreateTints = void(__fastcall)(const GameArray<TintMask*>& acTints, NiRenderedTexture* apTexture);

POINTER_SKYRIMSE(NiRTTI, NiMaskedShaderRTTI, 0x1431D1AF8 - 0x140000000);
POINTER_SKYRIMSE(TCreateTexture, CreateTexture, 0x140C68D20 - 0x140000000);
POINTER_SKYRIMSE(TCreateResourceView, CreateResourceView, 0x140D6DBC0 - 0x140000000);
POINTER_SKYRIMSE(TCreateTints, CreateTints, 0x1403DB420 - 0x140000000);
POINTER_SKYRIMSE(TextureHolder, s_textureHolder, 0x143028490 - 0x140000000);

void FaceGenSystem::Update(World& aWorld, Actor* apActor, FaceGenComponent& aFaceGenComponent) noexcept
{
    if (aFaceGenComponent.Generated)
        return;

    auto pTriBasedGeom = GetHeadTriBasedGeom(apActor, 1);

    if (!pTriBasedGeom) return;
    
    BSShaderProperty* pShaderProperty = niptr_cast<BSShaderProperty>(pTriBasedGeom->effect);

    if (!pShaderProperty) return;

    pShaderProperty->IncRef();

    BSLightingShaderProperty* pLightingShader = static_cast<BSLightingShaderProperty*>(pShaderProperty);

    if (pLightingShader->GetRTTI() == NiMaskedShaderRTTI.Get())
    {
        BSMaskedShaderMaterial* pMaterial = static_cast<BSMaskedShaderMaterial*>(pLightingShader->material);

        BSFixedString name("");
        auto pTexture = CreateTexture(name);
        pTexture->buffer = CreateResourceView(s_textureHolder.Get(), 512, 512);

        auto& tintsEntries = aFaceGenComponent.FaceTints.Entries;

        GameArray<TintMask*> tints;
        tints.capacity = tints.length = aFaceGenComponent.FaceTints.Entries.size() & 0xFFFFFFFF;
        tints.data = (TintMask**)Memory::Allocate(sizeof(TintMask*) * tints.length);

        for (auto i = 0u; i < tints.length; ++i)
        {
            tints[i] = Memory::New<TintMask>();

            tints[i]->alpha = tintsEntries[i].Alpha;
            tints[i]->color = tintsEntries[i].Color;
            tints[i]->type = tintsEntries[i].Type;

            auto pNewTexture = Memory::New<TESTexture>();
            pNewTexture->Construct();
            pNewTexture->Init();

            pNewTexture->name.Set(tintsEntries[i].Name.c_str());

            tints[i]->texture = pNewTexture;
        }

        CreateTints(tints, pTexture);

        for (auto i = 0u; i < tints.length; ++i)
        {
            tints[i]->texture->~TESTexture();
            Memory::Free(tints[i]->texture);
            Memory::Free(tints[i]);
        }

        Memory::Free(tints.data);
        
        pMaterial->renderedTexture = pTexture;

        aFaceGenComponent.Generated = true;
    }

    pShaderProperty->DecRef();
    
}

void FaceGenSystem::Setup(World& aWorld, const entt::entity aEntity, const Tints& acTints) noexcept
{
    if (acTints.Entries.empty())
        return;

    auto& component = aWorld.emplace<FaceGenComponent>(aEntity);
    component.FaceTints = acTints;
}

#else

void FaceGenSystem::Update(World& aWorld, Actor* apActor, FaceGenComponent& aFaceGenComponent) noexcept
{
    TP_UNUSED(aWorld);
    TP_UNUSED(apActor);
    TP_UNUSED(aFaceGenComponent);
}

void FaceGenSystem::Setup(World& aWorld, const entt::entity aEntity, const Tints& acTints) noexcept
{
    TP_UNUSED(aWorld);
    TP_UNUSED(aEntity);
    TP_UNUSED(acTints);
}


#endif
