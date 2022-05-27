#pragma once

class BSTimer
{
  public:
    BSTimer();

    static uint64_t GetHighPrecisionTime();

    float GetTimeSeconds();

    void Init(uint32_t auiTime);

    uint64_t GetHighPrecisionTickCount();

    inline uint64_t GetTimeMS()
    {
        return uiLastTime;
    }

  private:
    __int64 iHighPrecisionInitTime;
    float fClamp;
    float fClampRemainder;
    float fDelta;
    float fRealTimeDelta;
    uint64_t uiLastTime;
    uint64_t uiFirstTime;
    uint64_t uiDisabledLastTime;
    uint64_t uiDisabledFirstTime;
    uint32_t uiDisableCounter;
    bool bUseGlobalTimeMultiplierTarget = false;
};

static_assert(sizeof(BSTimer) == 0x40);
