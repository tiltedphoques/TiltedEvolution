
#include "Shared/Sky/Sky.h"
#include "Shared/Sky/Moon.h"

// TODO: fetch random from service...
static uint32_t myRandom = 1337;

uint32_t (*BSRandom_UnsignedInt)(void*, uint32_t){nullptr};

uint32_t Hook_BSRandom_UnsignedInt(void* apGenerator, uint32_t aMax)
{
    return myRandom;
}

static TiltedPhoques::Initializer s_SkyInit([]() {
    TiltedPhoques::SwapCall(0x1402C226F, BSRandom_UnsignedInt, &Hook_BSRandom_UnsignedInt);
});
