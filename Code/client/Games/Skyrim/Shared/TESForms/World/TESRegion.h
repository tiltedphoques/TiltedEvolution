
#include <Components/TESModel.h>
#include <Components/TESTexture.h>
#include <Forms/TESForm.h>

struct TESRegionDataList;

class TESWeather;

class TESRegionPoint
{
  public:
    NiPoint2 point; // 00
};
static_assert(sizeof(TESRegionPoint) == 0x8);

class TESRegionPointList : public GameList<TESRegionPoint> // RPLD
{
  public:
    struct ScaleResult
    {
        TESRegionPoint point; // 00
        float dist;           // 08
        float scale;          // 0C
    };
    static_assert(sizeof(ScaleResult) == 0x10);

    ScaleResult* pLastScaleResult; // 10
    bool bOwnsPointMemory;         // 18
    uint8_t pad19;                 // 19
    uint16_t pad1A;                // 1A
    NiPoint2 Minimums;             // 1C
    NiPoint2 Maximums;             // 24
    uint32_t iDistanceInsideAtMax; // 2C - RPLI
    uint32_t iCount;               // 30
    uint32_t pad34;                // 34
};
static_assert(sizeof(TESRegionPointList) == 0x38);

class TESRegion : public TESForm
{
  public:
    virtual ~TESRegion() = default;
    virtual bool Validate() = 0;

    // This will select a new random weather.
    TESWeather* SelectWeather();

    // apply new weather.
    void SetCurrentWeather(TESWeather*);

  private:
    TESRegionDataList* pDataList;              // 20
    GameList<TESRegionPointList>* pPointLists; // 28
    void* pWorldSpace;                         // 30 - WNAM
    TESWeather* pCurrentWeather;               // 38
    float EmittanceColor[4];                   // 40
};

static_assert(sizeof(TESRegion) == 0x50);
