#include <Services/CalendarService.h>
#include <TimeManager.h>

TimeData* TimeData::Get() noexcept
{
    POINTER_SKYRIMSE(TimeData*, s_instance, 400447);
    POINTER_FALLOUT4(TimeData*, s_instance, 1444953);
    return *(s_instance.Get());
}

using TSimulateTime = void(TimeData *, float);
static TSimulateTime *RealSimulateTime;

// in SP mode we let the client handle its own time simulation
void HookSimulateTime(TimeData *apData, float aMultiplier)
{
    if (CalendarService::AllowGameTick())
    {
        RealSimulateTime(apData, aMultiplier);
    }
}

static TiltedPhoques::Initializer s_loadingScreenHooks([]() {

    POINTER_SKYRIMSE(TSimulateTime, s_SimulateTime, 36291);
    POINTER_FALLOUT4(TSimulateTime, s_SimulateTime, 1330305);

    RealSimulateTime = s_SimulateTime.Get();
    TP_HOOK(&RealSimulateTime, HookSimulateTime);
});
