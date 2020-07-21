#include <Services/TestService.h>
#include <Services/ImguiService.h>
#include <Services/TransportService.h>

#include <Events/UpdateEvent.h>

#include <Games/References.h>

#include <Games/Skyrim/BSAnimationGraphManager.h>
#include <Games/Skyrim/Forms/TESNPC.h>

#include <Games/Fallout4/BSAnimationGraphManager.h>
#include <Games/Fallout4/Forms/TESNPC.h>

#include <Components.h>
#include <World.h>

#include <imgui.h>
#include <Games/SaveLoad.h>

extern thread_local bool g_overrideFormId;

void __declspec(noinline) TestService::PlaceActorInWorld() noexcept
{
    const auto pPlayerBaseForm = static_cast<TESNPC*>(PlayerCharacter::Get()->baseForm);

    String data;

    pPlayerBaseForm->Serialize(&data);

    char buffer[1 << 15];

    BGSSaveFormBuffer saveBuffer;
    saveBuffer.buffer = buffer;
    saveBuffer.capacity = 1 << 15;
    saveBuffer.changeFlags = 1024;

    PlayerCharacter::Get()->SaveInventory(&saveBuffer);

    BGSLoadFormBuffer loadBuffer(saveBuffer.changeFlags);
    loadBuffer.SetSize(saveBuffer.position);
    loadBuffer.buffer = buffer;
    loadBuffer.formId = 0;
    loadBuffer.form = nullptr;

    const auto pNpc = TESNPC::Create(data, pPlayerBaseForm->GetChangeFlags());
    auto pActor = Actor::Create(pPlayerBaseForm);
#ifdef TP_SKYRIM
    pActor->UnEquipAll();
#endif
    pActor->LoadInventory(&loadBuffer);

    m_actors.emplace_back(pActor);
}

TestService::TestService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport, ImguiService& aImguiService)
    : m_dispatcher(aDispatcher)
    , m_transport(aTransport)
    , m_world(aWorld)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&TestService::OnUpdate>(this);
    m_drawImGuiConnection = aImguiService.OnDraw.connect<&TestService::OnDraw>(this);
}

void TestService::RunDiff()
{
    if (m_actors.empty())
        return;

    auto pActor = m_actors[0];
    if(pActor)
    {
        BSAnimationGraphManager* pManager = nullptr;
        if (pActor->animationGraphHolder.GetBSAnimationGraph(&pManager))
        {
            pManager->DumpAnimationVariables();

            pManager->Release();
        }
    }
}

TestService::~TestService() noexcept = default;

void TestService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    static std::atomic<bool> s_f8Pressed = false;
    static std::atomic<bool> s_f7Pressed = false;

    //RunDiff();

    if (GetAsyncKeyState(VK_F8))
    {
        if (!s_f8Pressed)
        {
            s_f8Pressed = true;

            if(m_actors.empty())
                PlaceActorInWorld();
            else
            {
                BSAnimationGraphManager* pManager = nullptr;
                if(PlayerCharacter::Get()->animationGraphHolder.GetBSAnimationGraph(&pManager))
                {
                    pManager->DumpAnimationVariables();

                    pManager->Release();
                }
            }
        }
    }
    else
        s_f8Pressed = false;
}

void TestService::OnDraw() noexcept
{
    const auto view = m_world.view<FormIdComponent>();
    if (view.empty())
        return;

    ImGui::Begin("Server");

    static char s_address[256] = "127.0.0.1:10578";
    ImGui::InputText("Address", s_address, std::size(s_address));

    if(m_transport.IsOnline())
    {
        if (ImGui::Button("Disconnect"))
            m_transport.Close();
    }
    else
    {
        if (ImGui::Button("Connect"))
        {
            m_transport.Connect(s_address);
        }
    }

    ImGui::End();
}


