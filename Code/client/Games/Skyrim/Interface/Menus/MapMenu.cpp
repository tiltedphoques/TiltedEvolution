
#include <BSCore/BSTimer.h>
#include <Interface/Menus/MapMenu.h>
#include <Interface/UI.h>

namespace
{
BSTimer* mapTimer{nullptr};

UI_MESSAGE_RESULTS (*MapMenu_ProcessUiMessage)(MapMenu*, UIMessage*){nullptr};
void (*MapMenu_AdvanceMovie)(MapMenu*, float, int){nullptr};

constexpr uint64_t kPlayerMarkersUpdateTime = 1000;
}

UI_MESSAGE_RESULTS Hook_MapMenu_ProcessUiMessage(MapMenu* apSelf, UIMessage* apUiMessage)
{
    return MapMenu_ProcessUiMessage(apSelf, apUiMessage);
}

void Hook_MapMenu_AdvanceMovie(MapMenu* apSelf, float afInterval, int aCurrentTime)
{
    if (mapTimer->GetTimeMS() > kPlayerMarkersUpdateTime)
    {
        spdlog::info("Mapmarkers pulse");
    }

    MapMenu_AdvanceMovie(apSelf, afInterval, aCurrentTime);
}

static TiltedPhoques::Initializer s_init([]() {
    const VersionDbPtr<void*> vtableLoc(216501);
    void** vt = vtableLoc.Get();
    // at idx + 1 since the vt itself is our adl address
    MapMenu_ProcessUiMessage = reinterpret_cast<decltype(MapMenu_ProcessUiMessage)>(vt[5]);
    MapMenu_AdvanceMovie = reinterpret_cast<decltype(MapMenu_AdvanceMovie)>(vt[6]);

    vt[5] = &Hook_MapMenu_ProcessUiMessage;
    vt[6] = &Hook_MapMenu_AdvanceMovie;

    const VersionDbPtr<uint8_t> timerLoc(406688);
    mapTimer = reinterpret_cast<decltype(mapTimer)>(timerLoc.Get());

// Disabled because the mapmenu in first person breaks.
// I fix that later, but for now it doesn't break gameplay.
#if 0
    // https://github.com/Vermunds/SkyrimSoulsRE/blob/master/src/Menus/MapMenuEx.cpp
    // Of course this isnt perfect yet. but we"ll see

    VersionDbPtr<void*> hookLoc(53112);
    TiltedPhoques::Nop(hookLoc.GetPtrU() + 0x53, 4);
    TiltedPhoques::Nop(hookLoc.GetPtrU() + 0x9D, 2);
    TiltedPhoques::Nop(hookLoc.GetPtrU() + 0x9F, 1);
#endif
});
