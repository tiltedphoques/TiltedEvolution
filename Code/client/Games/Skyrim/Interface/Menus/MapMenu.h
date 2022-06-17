#pragma once

#include <Games/Events.h>
#include <Games/Primitives.h>
#include <Interface/IMenu.h>

class MenuOpenCloseEvent;

void SetWindowCursor(MapMenu* apMapMenu, float afCursorX, int afCursorY);
void AdvanceMovie(MapMenu* apMapMenu, float afInterval, int aCurrentTime);
void RefreshMarkers(MapMenu* apMapMenu);
void SetWaypoint(PlayerCharacter* apPlayer, NiPoint3* apPosition, TESWorldSpace* apWorldSpace);
void RemoveWaypoint(PlayerCharacter* apPlayer);

struct IMapCameraCallbacks
{
    // add
    virtual void Unk_00(void); // 00
    virtual void Unk_01(void); // 01

    virtual ~IMapCameraCallbacks(); // 02
};
static_assert(sizeof(IMapCameraCallbacks) == 0x8);

struct GFxValueXX
{
    void* ObjectInterface_0;
    int32 Type_8;
    char _pad_C[4];
    uint64 Value_10;
};

struct __declspec(align(4)) BSSoundHandle
{
    unsigned int iSoundID;
    bool bAssumeSuccess;
    unsigned __int8 eState;
};

struct ImageData
{
    void* unk00;            // 00 - smart ptr
    uint64_t unk08;         // 08
    BSFixedString filePath; // 10
};
static_assert(sizeof(ImageData) == 0x18);

struct LocalMapMenu
{
    class InputHandler;

    struct LocalMapCullingProcess
    {
        char pad0[0x30360];
    };
    static_assert(sizeof(LocalMapCullingProcess) == 0x30360);

    GameArray<void*> unk00000;                  // 00000
    GFxValueXX gfxVar_MarkerData;               // 00018
    float unk00030;                             // 00030
    float unk00034;                             // 00034
    float unk00038;                             // 00038
    float unk0003C;                             // 0003C
    LocalMapCullingProcess localCullingProcess; // 00040
    ImageData unk303A0;                         // 303A0
    GFxValueXX unk303B8;                        // 303B8
    GFxValueXX unk303D0;                        // 303D0
    void* unk303E8;                             // 303E8
    GamePtr<InputHandler> unk303F0;             // 303F0
    uint64_t unk303F8;                          // 303F8
};

struct MapCamera
{
    char pad0[0x90];
};

struct MapMenu final : public IMenu, public BSTEventSink<MenuOpenCloseEvent>, public IMapCameraCallbacks
{
    GamePtr<MapMoveHandler> moveHandler; // 00040
    GamePtr<MapLookHandler> lookHandler; // 00048
    GamePtr<MapZoomHandler> zoomHandler; // 00050
    uint64_t unk00058;                   // 00058
    LocalMapMenu localMapMenu;           // 00060
    uint32_t playerHandle_Handlemanager; // 30460
    uint32_t unk30464;                   // 30464
    uint32_t unk30468;                   // 30468
    uint32_t unk3046C;                   // 3046C
    // array of some sort of baseform
    GameArray<void*> mapForms; // 30470

    struct MarkerData
    {
        void* m1;
        int handle_to_testobjectref; // HandleManager handle
    };

    // array of mapmarker data
    GameArray<void*> mapMarkerData; // 30488

    MapCamera camera;                // 304A0
    uint64_t unk30530;               // 30530
    TESWorldSpace* worldSpace;       // 30538
    GFxValueXX MarkerDescriptionObj; // 30540
    float MarkerSelectionDist;       // 30558
    uint64_t unk30560;               // 30560
    uint64_t unk30568;               // 30568
    uint32_t unk30570;               // 30570
    BSSoundHandle unk30574;          // 30574
    uint32_t unk30580;               // 30580
    uint32_t unk30584;
    bool unk1; // 30588
    bool unk2;
    bool unk3;
    bool unk4;
    bool unk5;
    bool unk6;
    bool unk7;
    bool unk8;
    uint64_t unk30590; // 30590
};
static_assert(sizeof(MapMenu) == 0x30598);
