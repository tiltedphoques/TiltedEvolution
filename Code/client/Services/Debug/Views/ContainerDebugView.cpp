
#include <imgui.h>
#include <PlayerCharacter.h>
#include <Services/TestService.h>

void TestService::DrawContainerDebugView()
{
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("ExtraContainerChanges"))
        {
            static uint32_t s_selectedInvFormId = 0;

            auto* pContainerActor = PlayerCharacter::Get();

            const auto pContainerChanges = pContainerActor->GetContainerChanges()->entries;

            ImGui::BeginChild("Items", ImVec2(0, 200), true);

            int i = 0;
            for (auto pChange : *pContainerChanges)
            {
                if (pChange && pChange->form && pChange->dataList)
                {
                    char name[256];
                    sprintf_s(name, std::size(name), "%x", pChange->form->formID);

                    if (ImGui::Selectable(name, s_selectedInvFormId == pChange->form->formID))
                    {
                        s_selectedInvFormId = pChange->form->formID;
                    }
                }
            }

            ImGui::EndChild();

            int itemCount = 0;
            int dataListCount = 0;

            ImVec4 red{255.f, 0.f, 0.f, 255.f};
            ImVec4 green{0.f, 255.f, 0.f, 255.f};

            for (auto pChange : *pContainerChanges)
            {
                if (pChange && pChange->form && pChange->form->formID == s_selectedInvFormId)
                {
                    itemCount++;

                    const auto pDataLists = pChange->dataList;
                    for (auto* pDataList : *pDataLists)
                    {
                        if (pDataList)
                        {
                            dataListCount++;

                            char name[256];
                            sprintf_s(name, std::size(name), "Item %d", itemCount);

                            if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
                            {
                                BSScopedLock<BSRecursiveLock> _(pDataList->lock);

                                bool charge = pDataList->Contains(ExtraData::Charge);
                                ImGui::TextColored(charge ? green : red, "charge");
                                bool count = pDataList->Contains(ExtraData::Count);
                                ImGui::TextColored(count ? green : red, "count");
                                bool enchantment = pDataList->Contains(ExtraData::Enchantment);
                                ImGui::TextColored(enchantment ? green : red, "enchantment");
                                bool health = pDataList->Contains(ExtraData::Health);
                                ImGui::TextColored(health ? green : red, "health");
                                bool poison = pDataList->Contains(ExtraData::Poison);
                                ImGui::TextColored(poison ? green : red, "poison");
                                bool soul = pDataList->Contains(ExtraData::Soul);
                                ImGui::TextColored(soul ? green : red, "soul");
                                bool textDisplayData = pDataList->Contains(ExtraData::TextDisplayData);
                                ImGui::TextColored(textDisplayData ? green : red, "textDisplayData");
                                bool worn = pDataList->Contains(ExtraData::Worn);
                                ImGui::TextColored(worn ? green : red, "worn");
                                bool wornLeft = pDataList->Contains(ExtraData::WornLeft);
                                ImGui::TextColored(wornLeft ? green : red, "wornLeft");
                            }
                        }
                    }
                }
            }

            if (ImGui::CollapsingHeader("Metadata", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::InputInt("Item count", &itemCount, 0, 0,
                                ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::InputInt("Data list count", &dataListCount, 0, 0,
                                ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("TESContainer"))
        {
            static uint32_t s_selectedInvFormId = 0;

            PlayerCharacter* pPlayer = PlayerCharacter::Get();
            TESContainer* pContainer = pPlayer->GetContainer();
            if (pContainer && pContainer->entries)
            {
                ImGui::Text("Items (%d)", pContainer->count);

                ImGui::BeginChild("Items", ImVec2(0, 200), true);

                for (int i = 0; i < pContainer->count; ++i)
                {
                    auto pEntry = pContainer->entries[i];
                    if (!pEntry || !pEntry->form)
                        continue;

                    char name[256];
                    sprintf_s(name, std::size(name), "%x (%d)", pEntry->form->formID, pEntry->count);

                    if (ImGui::Selectable(name, s_selectedInvFormId == pEntry->form->formID))
                    {
                        s_selectedInvFormId = pEntry->form->formID;
                    }
                }

                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }
    }
}
