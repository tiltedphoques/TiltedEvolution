
#include "Services/DiscordService.h"
#include "Events/UpdateEvent.h"

#include <Filesystem.hpp>
#include <D3D11Hook.hpp>

#include <Games/Skyrim/PlayerCharacter.h>
#include <Games/Skyrim/Forms/TESWorldSpace.h>
#include <Games/Fallout4/PlayerCharacter.h>
#include <Games/Fallout4/Forms/TESWorldSpace.h>

#include <Events/LocationChangeEvent.h>

// TODO: codestyle
static IDiscordUserEvents cUserEvents = {

};

static IDiscordActivityEvents cActivityEvents = {

};

static IDiscordRelationshipEvents cRelationShipEvents = {

};

DiscordService::DiscordService(entt::dispatcher &aDispatcher)
{
    // initialize persistant rich presence data
    m_ActivityState.instance = false;
    m_ActivityState.type = EDiscordActivityType::DiscordActivityType_Playing;
    std::strcpy(m_ActivityState.details, "Loading");

#if TP_SKYRIM
    std::strcpy(m_ActivityState.assets.large_image, "placeholder");
#else
    std::strcpy(m_ActivityState.assets.large_image, "logo");
#endif

    // bind render events in order to draw the discord overlay
    // there is still some work to be done to bind keyboard input
    auto &d3d11 = TiltedPhoques::D3D11Hook::Get();
    d3d11.OnCreate.Connect([&](IDXGISwapChain *pSwapchain) { 
        InitOverlay(pSwapchain);
    });
    d3d11.OnPresent.Connect([&](IDXGISwapChain*) { 
        if (m_pOverlayMgr)
            m_pOverlayMgr->on_present(m_pOverlayMgr);
    });

    // world change event
    m_cellChangeConnection =
        aDispatcher.sink<LocationChangeEvent>().connect<&DiscordService::OnLocationChangeEvent>(this);
}

DiscordService::~DiscordService() = default;

void DiscordService::Create(entt::dispatcher &dispatch) noexcept
{
    entt::service_locator<DiscordService>::set(std::make_shared<DiscordService>(dispatch));
}

DiscordService &DiscordService::Get() noexcept
{
    return entt::service_locator<DiscordService>::ref();
}

void DiscordService::OnLocationChangeEvent() noexcept
{
    auto *pPlayer = PlayerCharacter::Get();

    // we'll disable this inbuilt location tracker
    // in case the user requests a custom discord presence
    if (!m_bCustomPresence && pPlayer)
    {
        //auto *pLocation = pPlayer->GetCurrentLocation();
        auto *pLocation = pPlayer->locationForm;
        auto *pWorldspace = pPlayer->GetWorldSpace();
        bool updateTimestamp = false;

        if (pLocation)
            std::strncpy(m_ActivityState.details, pLocation->GetName(), sizeof(DiscordActivity::details));

        if (pWorldspace)
        {
            if (pWorldspace->fullName.value.data)
                std::strncpy(m_ActivityState.state, pWorldspace->fullName.value.AsAscii(),
                             sizeof(DiscordActivity::state));

            if (m_lastWorldspaceId != pWorldspace->formID)
            {
                updateTimestamp = true;
                m_lastWorldspaceId = pWorldspace->formID;
            }
        }

        UpdatePresence(updateTimestamp);
    }
}

bool DiscordService::IsCanaryDiscord()
{
    DiscordBranch branch{};
    m_pAppMgr->get_current_branch(m_pAppMgr, &branch);
    return std::strcmp(branch, "master") != 0;
}

void DiscordService::UpdatePresence(bool newTimeStamp)
{
    if (m_pActivity)
    {
        if (newTimeStamp)
            m_ActivityState.timestamps.start = time(nullptr);

        m_pActivity->update_activity(m_pActivity, &m_ActivityState, nullptr, [](void *, EDiscordResult result) {
            if (result != EDiscordResult::DiscordResult_Ok)
            {
                std::printf("Failed to update discord presence %d\n", static_cast<int>(result));
            }
        });
    }
}

void DiscordService::InitOverlay(IDXGISwapChain *pSwapchain)
{
    if (!m_pOverlayMgr)
        return;

    bool enabled;
    m_pOverlayMgr->is_enabled(m_pOverlayMgr, &enabled);

    if (enabled)
    {
        auto result = m_pOverlayMgr->init_drawing_dxgi(m_pOverlayMgr, pSwapchain, true);

        // attempt to unlock it
        m_pOverlayMgr->set_locked(m_pOverlayMgr, false, nullptr, nullptr);
        std::printf("Enabled discord overlay! %d\n", static_cast<int>(result));
    }
}

bool DiscordService::Init()
{
    auto dllPath = TiltedPhoques::GetPath().wstring() + L"\\discord_game_sdk.dll";
    MessageBoxA(0, "Attach", 0, 0);
    // as we make the game sdk optional we need to dynamiclly resolve the export
    HMODULE pHandle = LoadLibraryW(dllPath.c_str());
    if (!pHandle)
        return false;

    auto *f_pDiscordCreate = (decltype(&DiscordCreate))(GetProcAddress(pHandle, "DiscordCreate"));

    if (!f_pDiscordCreate)
        return false;

    DiscordCreateParams params{};
    DiscordCreateParamsSetDefault(&params); // initialize version fields

#if TP_SKYRIM64
    params.client_id = 739600151277994076; //"Skyrim Together"
#else
    params.client_id = 739600293087674489; //"Fallout Together"
#endif

    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.event_data = this; // this is our userpointer
    params.user_events = &cUserEvents;
    params.relationship_events = &cRelationShipEvents;
    params.activity_events = &cActivityEvents;
    auto result = f_pDiscordCreate(DISCORD_VERSION, &params, &m_pCore);
    if (result != DiscordResult_Ok)
    {
        std::printf("Failed to create Discord instance. Error code: (%d)\n", static_cast<int>(result));
        FreeLibrary(pHandle);
        return false;
    }

#if 0
    m_pCore->set_log_hook(m_pCore, EDiscordLogLevel::DiscordLogLevel_Debug, this,
        [](void *, EDiscordLogLevel, const char *msg) { std::printf("NEW DISCORD MSG : %s\n", msg);
                          });
#endif

    m_pUserMgr = m_pCore->get_user_manager(m_pCore);
    m_pActivity = m_pCore->get_activity_manager(m_pCore);
    m_pAppMgr = m_pCore->get_application_manager(m_pCore);
    m_pOverlayMgr = m_pCore->get_overlay_manager(m_pCore);

    // set initial presence state
    m_ActivityState.application_id = params.client_id;
    UpdatePresence(true);

    //TODO (Force): i want to move this away from its own thread
    //this is done because discord needs to be ticked before world
    static std::thread updateThread([&]() { 
        while (true)
        {
            const auto runResult = m_pCore->run_callbacks(m_pCore);
            if (runResult != DiscordResult_Ok)
                break;

            //update at "60" fps 
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    });
    updateThread.detach();

    return true;
}

void DiscordService::Update()
{
}
