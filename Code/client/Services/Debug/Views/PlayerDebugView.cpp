#include <Services/DebugService.h>

#include <imgui.h>
#include <inttypes.h>
#include <Games/TES.h>

#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

void DebugService::DrawPlayerDebugView()
{
    PlayerCharacter* pPlayer = PlayerCharacter::Get();
    if (!pPlayer)
    {
        return;
    }

    ImGui::Begin("Player");

    auto pLeftWeapon = pPlayer->GetEquippedWeapon(0);
    auto pRightWeapon = pPlayer->GetEquippedWeapon(1);

    uint32_t leftId = pLeftWeapon ? pLeftWeapon->formID : 0;
    uint32_t rightId = pRightWeapon ? pRightWeapon->formID : 0;

    ImGui::InputScalar("Left Item", ImGuiDataType_U32, (void*)&leftId, nullptr, nullptr, nullptr,
                       ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("Right Item", ImGuiDataType_U32, (void*)&rightId, nullptr, nullptr, nullptr,
                       ImGuiInputTextFlags_ReadOnly);

    leftId = pPlayer->magicItems[0] ? pPlayer->magicItems[0]->formID : 0;
    rightId = pPlayer->magicItems[1] ? pPlayer->magicItems[1]->formID : 0;

    ImGui::InputScalar("Right Magic", ImGuiDataType_U32, (void*)&rightId, nullptr, nullptr, nullptr,
                       ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("Left Magic", ImGuiDataType_U32, (void*)&leftId, nullptr, nullptr, nullptr,
                       ImGuiInputTextFlags_ReadOnly);

#if TP_SKYRIM64
    auto* leftHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::LEFT_HAND);
    auto* rightHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::RIGHT_HAND);
    auto* otherHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::OTHER);
    auto* instantHandCaster = pPlayer->GetMagicCaster(MagicSystem::CastingSource::INSTANT);

    ImGui::InputScalar("leftHandCaster", ImGuiDataType_U64, (void*)&leftHandCaster, 0, 0, "%" PRIx64,
                       ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("rightHandCaster", ImGuiDataType_U64, (void*)&rightHandCaster, 0, 0, "%" PRIx64,
                       ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("otherHandCaster", ImGuiDataType_U64, (void*)&otherHandCaster, 0, 0, "%" PRIx64,
                       ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("instantHandCaster", ImGuiDataType_U64, (void*)&instantHandCaster, 0, 0, "%" PRIx64,
                       ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);

    ImGui::InputScalar("leftHandCasterSpell", ImGuiDataType_U64, (void*)&(leftHandCaster->pCurrentSpell), 0, 0,
                       "%" PRIx64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("rightHandCasterSpell", ImGuiDataType_U64, (void*)&(rightHandCaster->pCurrentSpell), 0, 0,
                       "%" PRIx64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("otherHandCasterSpell", ImGuiDataType_U64, (void*)&(otherHandCaster->pCurrentSpell), 0, 0,
                       "%" PRIx64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
    ImGui::InputScalar("instantHandCasterSpell", ImGuiDataType_U64, (void*)&(instantHandCaster->pCurrentSpell), 0, 0,
                       "%" PRIx64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
#endif

#if TP_SKYRIM64
    uint32_t shoutId = pPlayer->equippedShout ? pPlayer->equippedShout->formID : 0;

    ImGui::InputScalar("Shout", ImGuiDataType_U32, (void*)&shoutId, nullptr, nullptr, nullptr,
                       ImGuiInputTextFlags_ReadOnly);
#endif

    if (auto pWorldSpace = pPlayer->GetWorldSpace())
    {
        auto worldFormId = pWorldSpace->formID;
        ImGui::InputScalar("Worldspace", ImGuiDataType_U32, (void*)&worldFormId, nullptr, nullptr, "%" PRIx32,
                           ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    }
    if (auto pCell = pPlayer->GetParentCell())
    {
        auto cellFormId = pCell->formID;
        ImGui::InputScalar("Cell Id", ImGuiDataType_U32, (void*)&cellFormId, nullptr, nullptr, "%" PRIx32,
                           ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    }
    if (const auto playerParentCell = pPlayer->parentCell)
    {
        ImGui::InputScalar("Player parent cell", ImGuiDataType_U32, (void*)&playerParentCell->formID, nullptr, nullptr,
                           "%" PRIx32, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    }
    if (auto* pTES = TES::Get())
    {
        int32_t playerGrid[2] = {pTES->currentGridX, pTES->currentGridY};
        int32_t centerGrid[2] = {pTES->centerGridX, pTES->centerGridY};

        ImGui::InputInt2("Player grid", playerGrid, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputInt2("Center grid", centerGrid, ImGuiInputTextFlags_ReadOnly);
    }

    auto& modSystem = m_world.GetModSystem();

    // TODO: ft
#if TP_SKYRIM64
    if (ImGui::CollapsingHeader("Worn armor"))
    {
        Inventory wornArmor{};
        wornArmor = pPlayer->GetWornArmor();
        for (const auto& armor : wornArmor.Entries)
        {
            const uint32_t armorId = armor.BaseId.BaseId;
            ImGui::InputScalar("Item id", ImGuiDataType_U32, (void*)&armorId, nullptr, nullptr, "%" PRIx32,
                               ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
        }
    }

    ImGui::End();
#endif
}
