#pragma once

struct AITimer
{
    static float GetAITime()
    {
        static VersionDbPtr<float> s_value(2698609);
        return *s_value;
    }

    float fTargetTime;
    float fStartTime;
};
