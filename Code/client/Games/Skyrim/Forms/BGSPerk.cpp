#include "BGSPerk.h"
#include <Actor.h>

static TiltedPhoques::Initializer s_perkHooks([]() {

    // TODO: probably a better way to do this

    // in some TaskQueue function
    uint8_t mov1[] = { 0x44, 0x8B, 0x47, 0x18, 0x90 };
    void* destination1 = (void*)0x1405ECD14;
    memcpy(destination1, mov1, 5);

    // TaskQueueInterface::QueueApplyPerk()
    uint8_t mov2[] = { 0x44, 0x89, 0xCF, 0x90 };
    void* destination2 = (void*)0x1405EA76A;
    memcpy(destination2, mov2, 4);

    // TaskQueueInterface::QueueApplyPerk()
    uint8_t mov3[] = { 0x89, 0xF8, 0x90, 0x90 };
    void* destination3 = (void*)0x1405EA796;
    memcpy(destination3, mov3, 4);
});
