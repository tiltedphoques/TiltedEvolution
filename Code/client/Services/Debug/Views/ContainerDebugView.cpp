#include <imgui.h>
#include <inttypes.h>
#include <Services/DebugService.h>

#include <PlayerCharacter.h>
#include <EquipManager.h>
#include <World.h>

#include <DefaultObjectManager.h>

void DebugService::DrawContainerDebugView()
{
    static TESForm* pFetchForm = nullptr;
    static Actor* pActor = nullptr;

    ImGui::Begin("Inventory");

    ImGui::InputScalar("Form ID", ImGuiDataType_U32, &m_formId, 0, 0, "%" PRIx32, ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Look up"))
    {
        if (m_formId)
        {
            pFetchForm = TESForm::GetById(m_formId);
            if (pFetchForm)
                pActor = Cast<Actor>(pFetchForm);
        }
    }

    if (pActor)
    {
        static Inventory inventory{};

        if (ImGui::Button("Fetch inventory"))
            inventory = pActor->GetInventory();

        int inventoryCount = inventory.Entries.size();

        ImGui::InputInt("Inventory count", &inventoryCount, 0, 0, ImGuiInputTextFlags_ReadOnly);

        ImGui::BeginChild("Items", ImVec2(0, 200), true);

        for (Inventory::Entry& entry : inventory.Entries)
        {
            std::string itemLabel = fmt::format("{:X}", entry.BaseId.BaseId + entry.BaseId.ModId);
            if (!ImGui::CollapsingHeader(itemLabel.c_str()))
                continue;

            if (ImGui::Button("Equip"))
            {
                World::Get().GetRunner().Queue(
                    [entry, actorId = pActor->formID]()
                    {
                        auto& modSystem = World::Get().GetModSystem();
                        uint32_t itemId = modSystem.GetGameId(entry.BaseId);
                        TESForm* pItem = TESForm::GetById(itemId);
                        Actor* pActor = Cast<Actor>(TESForm::GetById(actorId));
                        EquipManager::Get()->Equip(pActor, pItem, nullptr, entry.Count, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false);
                    });
            }
            if (ImGui::Button("Unequip"))
            {
                World::Get().GetRunner().Queue(
                    [entry, actorId = pActor->formID]()
                    {
                        auto& modSystem = World::Get().GetModSystem();
                        uint32_t itemId = modSystem.GetGameId(entry.BaseId);
                        TESForm* pItem = TESForm::GetById(itemId);
                        Actor* pActor = Cast<Actor>(TESForm::GetById(actorId));
                        EquipManager::Get()->UnEquip(pActor, pItem, nullptr, entry.Count, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false, nullptr);
                    });
            }

            int itemCount = entry.Count;
            ImGui::InputInt("Item count", &itemCount, 0, 0, ImGuiInputTextFlags_ReadOnly);

            int isWorn = entry.ExtraWorn;
            ImGui::InputInt("Is worn?", &isWorn, 0, 0, ImGuiInputTextFlags_ReadOnly);

            int isWornLeft = entry.ExtraWornLeft;
            ImGui::InputInt("Is worn left?", &isWornLeft, 0, 0, ImGuiInputTextFlags_ReadOnly);

            int isWeapon = entry.EnchantData.IsWeapon;
            ImGui::InputInt("Is weapon?", &isWeapon, 0, 0, ImGuiInputTextFlags_ReadOnly);

            ImGui::InputFloat("Health", &entry.ExtraHealth, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

            int poisonId = entry.ExtraPoisonId.BaseId + entry.ExtraPoisonId.ModId;
            ImGui::InputInt("Poison ID", &poisonId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

            int poisonCount = entry.ExtraPoisonCount;
            ImGui::InputInt("Poison count", &poisonCount, 0, 0, ImGuiInputTextFlags_ReadOnly);

            int soulLevel = entry.ExtraSoulLevel;
            ImGui::InputInt("Soul level", &soulLevel, 0, 0, ImGuiInputTextFlags_ReadOnly);

            ImGui::InputFloat("Charge", &entry.ExtraCharge, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

            int enchantId = entry.ExtraEnchantId.BaseId + entry.ExtraEnchantId.ModId;
            ImGui::InputInt("Enchant ID", &enchantId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

            int enchantCharge = entry.ExtraEnchantCharge;
            ImGui::InputInt("Enchant charge", &enchantCharge, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);

            int isEnchantRemoveUnequip = entry.ExtraEnchantRemoveUnequip;
            ImGui::InputInt("Remove enchant on unequip?", &isEnchantRemoveUnequip, 0, 0, ImGuiInputTextFlags_ReadOnly);
            if (ImGui::CollapsingHeader("Effects"))
            {
                for (Inventory::EffectItem& effect : entry.EnchantData.Effects)
                {
                    std::string effectLabel = fmt::format("{:X}", effect.EffectId.BaseId + effect.EffectId.ModId);
                    if (!ImGui::CollapsingHeader(effectLabel.c_str()))
                        continue;

                    ImGui::InputFloat("Magnitude", &effect.Magnitude, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    int area = effect.Area;
                    ImGui::InputInt("Area", &area, 0, 0, ImGuiInputTextFlags_ReadOnly);
                    int duration = effect.Duration;
                    ImGui::InputInt("Duration", &duration, 0, 0, ImGuiInputTextFlags_ReadOnly);
                    ImGui::InputFloat("Raw cost", &effect.RawCost, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                }
            }

            ImGui::Separator();
        }

        ImGui::EndChild();
    }

    ImGui::End();
}
