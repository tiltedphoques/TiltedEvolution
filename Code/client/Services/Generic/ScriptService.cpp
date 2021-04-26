#include <TiltedOnlinePCH.h>

#include <Services/ScriptService.h>
#include <Services/ImguiService.h>
#include <Services/TransportService.h>

#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>

#include <World.h>
#include <Components.h>

#include <Actor.h>
#include <Forms/TESForm.h>

#include <Events/EventDispatcher.h>
#include <Messages/ClientRpcCalls.h>

#include <imgui.h>

ScriptService::ScriptService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService, TransportService& aTransportService) noexcept
    : ScriptStore(false)
    , m_dispatcher(aDispatcher)
    , m_world(aWorld)
    , m_transport(aTransportService)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&ScriptService::OnUpdate>(this);
    m_scriptsConnection = m_dispatcher.sink<Scripts>().connect<&ScriptService::OnScripts>(this);
    m_replicatedInitConnection = m_dispatcher.sink<FullObjects>().connect < &ScriptService::OnNetObjectsInitalize >(this);
    m_replicatedUpdateConnection = m_dispatcher.sink<Objects>().connect < &ScriptService::OnNetObjectsUpdate >(this);

    m_connectedConnection = m_dispatcher.sink<ConnectedEvent>().connect<&ScriptService::OnConnected>(this);
    m_disconnectedConnection = m_dispatcher.sink<DisconnectedEvent>().connect<&ScriptService::OnDisconnected>(this);

    m_drawImGuiConnection = aImguiService.OnDraw.connect<&ScriptService::OnDraw>(this);
}

ScriptService::~ScriptService() noexcept
{
    
}

void ScriptService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{

    ClientRpcCalls message;

    Buffer buff(10000);
    Buffer::Writer writer(&buff);

    const auto result = GetNetState()->GenerateCallRequest(writer);
    if(result)
    {
        message.Data.assign(reinterpret_cast<const char*>(buff.GetData()), writer.Size());
        m_transport.Send(message);
    }
}

void ScriptService::OnDraw() noexcept
{
    const auto view = m_world.view<FormIdComponent>();
    if (view.empty())
        return;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin("Engine");

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Scripts"))
        {
            DisplayNetObjects();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World"))
        {
            DisplayEntities();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

void ScriptService::OnScripts(const Scripts& acScripts) noexcept
{
    Buffer buff(reinterpret_cast<const uint8_t*>(acScripts.Data.data()), acScripts.Data.size());
    Buffer::Reader reader(&buff);

    GetNetState()->LoadDefinitions(reader);
}

void ScriptService::OnNetObjectsInitalize(const FullObjects& acNetObjects) noexcept
{
    Buffer buff(reinterpret_cast<const uint8_t*>(acNetObjects.Data.data()), acNetObjects.Data.size());
    Buffer::Reader reader(&buff);

    GetNetState()->LoadFullSnapshot(reader);
}

void ScriptService::OnNetObjectsUpdate(const Objects& acNetObjects) noexcept
{
    Buffer buff(reinterpret_cast<const uint8_t*>(acNetObjects.Data.data()), acNetObjects.Data.size());
    Buffer::Reader reader(&buff);

    GetNetState()->ApplyDifferentialSnapshot(reader);
}

void ScriptService::OnConnected(const ConnectedEvent&) noexcept
{
}

void ScriptService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    Reset();
}

void ScriptService::DisplayNetObject(NetObject* apObject)
{
    uintptr_t ptr = reinterpret_cast<uintptr_t>(apObject);

    if (apObject && ImGui::TreeNode((void*)ptr, "System", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto parentId = apObject->GetParentId();
        ImGui::InputScalarN("Parent Id", ImGuiDataType_S32, &parentId, 1, nullptr, nullptr, "%x", ImGuiInputTextFlags_ReadOnly);

        ImGui::TreePop();
    }

    if (apObject && ImGui::TreeNode((void*)(ptr+1), "Properties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const auto& definition = apObject->GetDefinition();
        apObject->GetProperties().Visit([&definition](const NetProperty* apProperty)
        {
            auto& def = definition.GetReplicatedProperty(apProperty->GetId());
            auto obj = apProperty->Get();
            switch (obj.get_type())
            {
            case sol::type::string:
                {
                    auto stringVal = apProperty->Get().as<std::string>();
                    ImGui::InputText(def.Name.c_str(), stringVal.data(), stringVal.size(), ImGuiInputTextFlags_ReadOnly);
                }
                break;
            case sol::type::boolean:
                {
                    auto boolVal = apProperty->Get().as<bool>();
                    ImGui::Checkbox(def.Name.c_str(), &boolVal);
                }
                break;
            case sol::type::number:
                {
                    float floatVal = static_cast<float>(apProperty->Get().as<double>());
                    ImGui::InputScalarN(def.Name.c_str(), ImGuiDataType_Float, &floatVal, 1, nullptr, nullptr, "%f", ImGuiInputTextFlags_ReadOnly);
                }
                break;
            default:
                spdlog::error("Unsupported replicated property type");
                break;
            }
        });

        ImGui::TreePop();
    }

    if (apObject && ImGui::TreeNode((void*)(ptr + 2), "RPCs", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto& definition = apObject->GetDefinition();

        for (auto& kvp : definition.GetRemoteProcedures())
        {
            auto& name = kvp.second.Name;
            ImGui::InputText("", const_cast<char*>(name.data()), kvp.first.size(), ImGuiInputTextFlags_ReadOnly);
        }

        ImGui::TreePop();
    }
}

void ScriptService::DisplayNetObjects() noexcept
{
    ImGui::Text("Replicated Objects");

    ImGui::BeginChild("ReplicatedObjects", ImVec2(0, 200), true);

    static uint32_t s_selectedId = 0;
    bool visited = false;

    GetNetState()->Visit([this, &visited](NetObject* apObject)
    {
        const auto fullname = apObject->GetDefinition().GetDisplayName();

        char name[256];
        sprintf_s(name, std::size(name), "%s (%x)", fullname.c_str(), apObject->GetId());

        if (apObject->GetId() == s_selectedId)
            visited = true;

        if(ImGui::Selectable(name, apObject->GetId() == s_selectedId))
        {
            s_selectedId = apObject->GetId();
        }
    });

    ImGui::EndChild();

    if(visited)
    {
        NetObject* pObject = GetNetState()->GetById(s_selectedId);

        DisplayNetObject(pObject);
    }
}

void ScriptService::DisplayEntities() noexcept
{
    static uint32_t s_selectedFormId = 0;
    static uint32_t s_selected = 0;

    StackAllocator<1 << 12> allocator;
    ScopedAllocator _{ allocator };

    const auto view = m_world.view<FormIdComponent>();

    Vector<entt::entity> entities(view.begin(), view.end());

    ImGui::Text("Actor list (%d)", entities.size());

    ImGui::BeginChild("Entities", ImVec2(0, 200), true);

    int i = 0;
    for(auto it : entities)
    {
        auto& formComponent = view.get<FormIdComponent>(it);
        const auto pActor = RTTI_CAST(TESForm::GetById(formComponent.Id), TESForm, Actor);

        if (!pActor || !pActor->baseForm)
            continue;

        char name[256];
        sprintf_s(name, std::size(name), "%s (%x)", pActor->baseForm->GetName(), formComponent.Id);

        if (ImGui::Selectable(name, s_selectedFormId == formComponent.Id))
        {
            s_selectedFormId = formComponent.Id;
        }

        if(s_selectedFormId == formComponent.Id)
            s_selected = i;

        ++i;
    }

    ImGui::EndChild();

    if (s_selected < entities.size())
        DisplayEntityPanel(entities[s_selected]);
}

void ScriptService::DisplayEntityPanel(entt::entity aEntity) noexcept
{
    const auto pFormIdComponent = m_world.try_get<FormIdComponent>(aEntity);
    const auto pLocalComponent = m_world.try_get<LocalComponent>(aEntity);
    const auto pRemoteComponent = m_world.try_get<RemoteComponent>(aEntity);

    if (pFormIdComponent)               DisplayFormComponent(*pFormIdComponent);
    if (pLocalComponent)                DisplayLocalComponent(*pLocalComponent);
    if (pRemoteComponent)               DisplayRemoteComponent(*pRemoteComponent);

    std::optional<uint32_t> id;
    if (pLocalComponent) id = pLocalComponent->Id;
    if (pRemoteComponent) id = pRemoteComponent->Id;

    if (id)
    {
        GetNetState()->Visit([this, id](NetObject* apObject)
        {
            if (apObject->GetParentId() == *id)
            {
                const auto fullname = apObject->GetDefinition().GetDisplayName();

                char name[256];
                sprintf_s(name, std::size(name), "%s (%x)", fullname.c_str(), apObject->GetId());

                if(ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    DisplayNetObject(apObject);
                }
            }
        });
    }
}

void ScriptService::DisplayFormComponent(FormIdComponent& aFormComponent) const noexcept
{
    if (!ImGui::CollapsingHeader("Form Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    const auto pActor = RTTI_CAST(TESForm::GetById(aFormComponent.Id), TESForm, Actor);

    ImGui::InputInt("Game Id", (int*)&aFormComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputFloat3("Position", pActor->position.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat3("Rotation", pActor->rotation.AsArray(), "%.3f", ImGuiInputTextFlags_ReadOnly);
    int isDead = int(pActor->IsDead());
    ImGui::InputInt("Is dead?", &isDead, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
}

void ScriptService::DisplayLocalComponent(LocalComponent& aLocalComponent) const noexcept
{
    if (!ImGui::CollapsingHeader("Local Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    auto& action = aLocalComponent.CurrentAction;
    ImGui::InputInt("Net Id", (int*)&aLocalComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("Action Id", (int*)&action.ActionId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputInt("Idle Id", (int*)&action.IdleId, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::InputScalarN("State", ImGuiDataType_U32, &action.State1, 2, nullptr, nullptr, "%x", ImGuiInputTextFlags_ReadOnly);
}

void ScriptService::DisplayRemoteComponent(RemoteComponent& aLocalComponent) const noexcept
{
    if (!ImGui::CollapsingHeader("Remote Component", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::InputInt("Server Id", (int*)&aLocalComponent.Id, 0, 0, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsHexadecimal);
}

void ScriptService::RegisterExtensions(ScriptContext& aContext)
{
    ScriptStore::RegisterExtensions(aContext);

    // Load extensions
}
