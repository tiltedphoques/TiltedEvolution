
#include <BSCore/BSTimer.h>

static uint64_t* BSTimer_uiFrequency = nullptr;

BSTimer::BSTimer()
{
    Init(0);
}

void BSTimer::Init(uint32_t auiTime)
{
    // we exclude
    // if ( !BSTimer::uiFrequency )
    // {
    //     QueryPerformanceFrequency(&Frequency);
    //     BSTimer::uiFrequency = Frequency.QuadPart;
    // }

    uiDisableCounter = 0;
    fClamp = 0.f;
    fClampRemainder = 0.f;
    fDelta = 0.f;
    fRealTimeDelta = 0.f;

    auto time = GetHighPrecisionTime();
    iHighPrecisionInitTime = time;

    uiFirstTime = auiTime ? auiTime : GetHighPrecisionTickCount();
    bUseGlobalTimeMultiplierTarget = false;
}

uint64_t BSTimer::GetHighPrecisionTickCount()
{
    return 1000 * (GetHighPrecisionTime() - iHighPrecisionInitTime) / *BSTimer_uiFrequency;
}

uint64_t BSTimer::GetHighPrecisionTime()
{
    LARGE_INTEGER PerformanceCount = {};
    QueryPerformanceCounter(&PerformanceCount);
    return PerformanceCount.QuadPart;
}

float BSTimer::GetTimeSeconds()
{
    if (uiLastTime < 0)
        uiLastTime = uiLastTime + 1.8446744e19; // 2^ 64
    return uiLastTime * 0.001;
}

static TiltedPhoques::Initializer s_initBSTimer([]() {
    const VersionDbPtr<uint8_t> freqLoc(410194);
    BSTimer_uiFrequency = reinterpret_cast<decltype(BSTimer_uiFrequency)>(freqLoc.Get());
});
