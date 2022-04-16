
#include <Games/Skyrim/BSRandom/BSRandom.h>

namespace BSRandom
{
static void* (*GetGenerator)();
static uint32_t (*Real_UnsignedInt)(void*, uint32_t);

uint32_t UnsignedInt(uint32_t aMin, uint32_t aMax)
{
    return Real_UnsignedInt(GetGenerator(), aMax - aMin);
}

static TiltedPhoques::Initializer s_randomInit([]() {
    const VersionDbPtr<void> unsignedInt(68276);
    Real_UnsignedInt = static_cast<decltype(Real_UnsignedInt)>(unsignedInt.GetPtr());

    const VersionDbPtr<void> getGenerator(14774);
    GetGenerator = static_cast<decltype(GetGenerator)>(getGenerator.GetPtr());
});

} // namespace BSRandom
