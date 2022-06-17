
#include <BSCore/BSTimer.h>
#include <Events/PlayerMapMarkerUpdateEvent.h>
#include <Events/PlayerSetWaypointEvent.h>
#include <Events/PlayerDelWaypointEvent.h>
#include <Interface/Menus/MapMenu.h>
#include <Interface/UI.h>
#include <TiltedOnlinePCH.h>
#include <PlayerCharacter.h>
#include <World.h>

namespace
{
BSTimer* mapTimer{nullptr};

UI_MESSAGE_RESULTS (*MapMenu_ProcessUiMessage)(MapMenu*, UIMessage*){nullptr};
void (*MapMenu_AdvanceMovie)(MapMenu*, float, int){nullptr};
void (*MapMenu_RefreshMarkers)(MapMenu*){nullptr};

TP_THIS_FUNCTION(MapMenu_SetWaypoint, void, PlayerCharacter, NiPoint3* apPosition, TESWorldSpace* apWorldSpace);
static MapMenu_SetWaypoint* RealSetWaypoint = nullptr;

TP_THIS_FUNCTION(MapMenu_RemoveWaypoint, void, PlayerCharacter);
static MapMenu_RemoveWaypoint* RealRemoveWaypoint = nullptr;

TP_THIS_FUNCTION(MapMenu_SetWindowCursor, void, MapMenu, float afCursorX, float afCursorY);
static MapMenu_SetWindowCursor* RealSetWindowCursor = nullptr;

void (*Hack_Dorefresh)(void*, const char*, void*);

constexpr uint64_t kPlayerMarkersUpdateTime = 1000;
} // namespace

void SetWindowCursor(MapMenu* apMapMenu, float afCursorX, int afCursorY)
{
    ThisCall(RealSetWindowCursor, apMapMenu, afCursorX, afCursorY);
}

void AdvanceMovie(MapMenu* apMapMenu, float afInterval, int aCurrentTime)
{
    ThisCall(MapMenu_AdvanceMovie, apMapMenu, afInterval, aCurrentTime);
}

void RefreshMarkers(MapMenu* apMapMenu)
{
    ThisCall(MapMenu_RefreshMarkers, apMapMenu);
}

void RemoveWaypoint(PlayerCharacter* apPlayer)
{
    ThisCall(RealRemoveWaypoint, apPlayer);
}

void SetWaypoint(PlayerCharacter* apPlayer, NiPoint3* apPosition, TESWorldSpace* apWorldSpace)
{
    ThisCall(RealSetWaypoint, apPlayer, apPosition, apWorldSpace);
}

UI_MESSAGE_RESULTS Hook_MapMenu_ProcessUiMessage(MapMenu* apSelf, UIMessage* apUiMessage)
{
    return MapMenu_ProcessUiMessage(apSelf, apUiMessage);
}

void Hook_MapMenu_AdvanceMovie(MapMenu* apSelf, float afInterval, int aCurrentTime)
{
    if (mapTimer->GetTimeMS() > kPlayerMarkersUpdateTime)
    {
        World::Get().GetRunner().Trigger(PlayerMapMarkerUpdateEvent());

#if 0
        char stack[0x40]{};
        *(void**)(&stack) = (void*)0x14179EF28;

        void* address = (*(uint8_t**)apSelf->uiMovie) + 144i64;
        ((void (*)(void*, int, const char*))address)(apSelf->uiMovie, 5, "WorldMap.MarkerData");

        Hack_Dorefresh(apSelf->uiMovie, "RefreshMarkers", &stack);
#endif

        // MapMenu_RefreshMarkers(apSelf);
        //*(uint8_t*)((uint8_t*)apSelf + 0x30498) = 1;
    }

    MapMenu_AdvanceMovie(apSelf, afInterval, aCurrentTime);
}

void TP_MAKE_THISCALL(HookSetWaypoint, PlayerCharacter, NiPoint3* apPosition, TESWorldSpace* apWorldSpace)
{
    Vector3_NetQuantize Position = {};
    Position.x = apPosition->x;
    Position.y = apPosition->y;

    World::Get().GetRunner().Trigger(PlayerSetWaypointEvent(Position));

    ThisCall(RealSetWaypoint, apThis, apPosition, apWorldSpace);
}

void TP_MAKE_THISCALL(HookRemoveWaypoint, PlayerCharacter)
{
    World::Get().GetRunner().Trigger(PlayerDelWaypointEvent());
    ThisCall(RealRemoveWaypoint, apThis);
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

    MapMenu_RefreshMarkers = reinterpret_cast<decltype(MapMenu_RefreshMarkers)>(0x1409154B0);

    POINTER_SKYRIMSE(MapMenu_SetWaypoint, s_setWaypoint, 40535);
    RealSetWaypoint = s_setWaypoint.Get();
    TP_HOOK(&RealSetWaypoint, HookSetWaypoint);

    POINTER_SKYRIMSE(MapMenu_RemoveWaypoint, s_removeWaypoint, 40536);
    RealRemoveWaypoint = s_removeWaypoint.Get();
    TP_HOOK(&RealRemoveWaypoint, HookRemoveWaypoint);

    POINTER_SKYRIMSE(MapMenu_SetWindowCursor, s_setWindowCursor, 53109);
    RealSetWindowCursor = s_setWindowCursor.Get();

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
