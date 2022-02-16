
#include <Interface/Menus/MapMenu.h>

static TiltedPhoques::Initializer s_init([]() {
    // https://github.com/Vermunds/SkyrimSoulsRE/blob/master/src/Menus/MapMenuEx.cpp
    // Of course this isnt perfect yet. but we"ll see

    VersionDbPtr<void*> hookLoc(53112);
    TiltedPhoques::Nop(hookLoc.GetPtrU() + 0x53, 4);
    TiltedPhoques::Nop(hookLoc.GetPtrU() + 0x9D, 2);
    TiltedPhoques::Nop(hookLoc.GetPtrU() + 0x9F, 1);
});
