#include <TiltedOnlinePCH.h>


#include "Services/DiscordService.h"
#include "Events/UpdateEvent.h"

#include <TiltedCore/Filesystem.hpp>
#include <PlayerCharacter.h>
#include <Forms/TESWorldSpace.h>

#include <base/threading/ThreadUtils.h>
#include <Events/LocationChangeEvent.h>

#define DISCORD_OVERLAY_ENABLE 0

IDiscordUserEvents DiscordService::s_mUserEvents = {
    DiscordService::OnUserUpdate
};

DiscordService::DiscordService(entt::dispatcher &aDispatcher)
{
    // initialize persistant rich presence data
    m_ActivityState.instance = false;
    m_ActivityState.type = EDiscordActivityType::DiscordActivityType_Playing;

    strcpy_s(m_ActivityState.details, "Loading");
    strcpy_s(m_ActivityState.assets.large_image, "logo");

    m_cellChangeConnection =
        aDispatcher.sink<LocationChangeEvent>().connect<&DiscordService::OnLocationChangeEvent>(this);

#if DISCORD_OVERLAY_ENABLE
    auto &d3d11 = TiltedPhoques::D3D11Hook::Get();
    d3d11.OnCreate.Connect([&](IDXGISwapChain *pSwapchain) { InitOverlay(pSwapchain); });
    d3d11.OnPresent.Connect([&](IDXGISwapChain *) { 
        if (m_bOverlayEnabled)
        {
            m_pOverlayMgr->on_present(m_pOverlayMgr);
        }
    });
#endif
}

DiscordService::~DiscordService() = default;

void DiscordService::OnUserUpdate(void* userp)
{
    auto* pDiscord = static_cast<DiscordService*>(userp);
    pDiscord->m_pUserMgr->get_current_user(pDiscord->m_pUserMgr, &pDiscord->m_userData);
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
            strncpy_s(m_ActivityState.details, pLocation->GetName(), sizeof(DiscordActivity::details));

        if (pWorldspace)
        {
            if (pWorldspace->fullName.value.data)
                strncpy_s(m_ActivityState.state, pWorldspace->fullName.value.AsAscii(),
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

void DiscordService::UpdatePresence(bool newTimeStamp)
{
    if (m_pActivity)
    {
        if (newTimeStamp)
            m_ActivityState.timestamps.start = time(nullptr);

        m_pActivity->update_activity(m_pActivity, &m_ActivityState, nullptr, [](void *, EDiscordResult result) {
            if (result != EDiscordResult::DiscordResult_Ok)
            {
                spdlog::error("Failed to update discord presence ({})", static_cast<int>(result));
            }
        });
    }
}

void DiscordService::InitOverlay(IDXGISwapChain *pSwapchain)
{
    m_pOverlayMgr->is_enabled(m_pOverlayMgr, &m_bOverlayEnabled);

    // attempt to unlock it
    m_pOverlayMgr->set_locked(m_pOverlayMgr, false, nullptr, nullptr);
   // spdlog::info("Enabled discord overlay! ({})", static_cast<int>(result));
}

void DiscordService::WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // this is a hack to force the auxiliary discord msg loop thread
    // to quit in a timely manner
    if (msg == WM_QUIT)
    {
        m_bRequestThreadKillHack = true;
        m_bOverlayEnabled = false;
        return;
    }
  
#if DISCORD_OVERLAY_ENABLE
    if (!m_bOverlayEnabled)
        return;

    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP: {
        bool down = ((msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN));
        if (down && wParam == VK_F6)
        {
            m_pOverlayMgr->set_locked(m_pOverlayMgr, false, nullptr, [](void *, EDiscordResult result) { 
                spdlog::info("unlocking discord overlay ({})", static_cast<int>(result));
                });
        }

      /*  if (wParam < 256)
            m_pOverlayMgr->key_event(m_pOverlayMgr, down, reinterpret_cast<const char *>(wParam),
                                     EDiscordKeyVariant::DiscordKeyVariant_Normal);*/
        break;
    }
    default:
        return;
    }
#endif
}

bool DiscordService::Init()
{
    auto dllPath = TiltedPhoques::GetPath().wstring() + L"\\discord_game_sdk.dll";

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
    params.user_events = &s_mUserEvents;
    auto result = f_pDiscordCreate(DISCORD_VERSION, &params, &m_pCore);

    if (result != DiscordResult_Ok || !m_pCore)
    {
        spdlog::error("Failed to create Discord instance ({})", static_cast<int>(result));
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

    if (!m_pUserMgr || !m_pActivity || !m_pAppMgr || !m_pOverlayMgr)
    {
        m_pCore->destroy(m_pCore);
        FreeLibrary(pHandle);
        return false;
    }

    // set initial presence state
    m_ActivityState.application_id = params.client_id;
    UpdatePresence(true);

    //TODO (Force): i want to move this away from its own thread
    //this is done because discord needs to be ticked before world
    static std::thread updateThread([&]() { 
        Base::SetCurrentThreadName("DiscordCallbacks");

        while (!m_bRequestThreadKillHack)
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
